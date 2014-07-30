
const byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
//char website[] PROGMEM = "brew-o-matic.eu01.aws.af.cm";
char website[] PROGMEM = "192.168.1.37";
// ethernet interface ip address
static byte myip[] = { 192,168,1,10 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };

static byte hisip[] = { 192,168,1,37 };
byte Ethernet::buffer[900];//900
Stash stash;
byte state = 0;
byte session_id;
static BufferFiller bfill;

void ethernet_setup(){
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, pin_ethernet_cs) == 0){
    
    Serial.println( "Failed to access Ethernet controller");
  }
  else{
    ether.staticSetup(myip, gwip);
    //if (!ether.dhcpSetup())
    //  Serial.println("DHCP failed");
  
    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);  
    ether.printIp("DNS: ", ether.dnsip);  
    
    ether.copyIp(ether.hisip, hisip);
    //ether.printIp("Server: ", ether.hisip);
    
    //if (!ether.dnsLookup(website))
    //  Serial.println("DNS failed");
    //ether.parseIp(ether.hisip, website);
    while (ether.clientWaitingGw())
      ether.packetLoop(ether.packetReceive());
    Serial.println("Gateway found");
      
    ether.printIp("SRV: ", ether.hisip);
  }
}

void ethernet_loop(){
  if(state == 1){
    ether.packetLoop(ether.packetReceive());
    const char* reply = ether.tcpReply(session_id);
    if(reply != 0){ 
      //Serial.println(F(" <<< Response Received"));
      //Serial.println(reply);   
    }
  }
  word len = ether.packetReceive(); 
  word pos = ether.packetLoop(len); 
  
  if(pos) { 
    Serial.println(1);    
    bfill = ether.tcpOffset();
    Serial.println(2);
    char* data = (char *) Ethernet::buffer + pos;
    Serial.println(3);
    // receive buf hasn't been clobbered by reply yet
    if (checkUrl(F("GET / "), data))
      homePage(bfill);
    else if (checkUrl(F("GET /main.css "), data))
      return; //mainCss(bfill);
    else if (checkUrl(F("GET /main.js "), data))
      return;//mainJs(bfill);
    else if (checkUrl(F("GET /list.json "), data))
      return;//listJson(bfill);     
    else
      bfill.print(F(
        "HTTP/1.0 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>404 Not Found</h1>"));  
    ether.httpServerReply(bfill.position()); // send web page data
  }
}

void ethernet_post_data(){
  state = 1;
  byte sd = stash.create();
  stash.print("{");
  stash.print("\"code\": \"1234\",");
  stash.print("\"coldStatus\": true,");
  stash.print("\"heatStatus\": false,");
  stash.print("\"source\": \"1\",");
  stash.print("\"temperature\": 22,");
  stash.print("\"temperatureMax\": 20,");
  stash.print("\"temperatureMin\": 19,");
  stash.print("\"temperatureExt\": 25");
  stash.print("}");
  stash.save();
  Stash::prepare(PSTR("POST /test.php HTTP/1.1\r\n" 
                        "Host: $F" "\r\n" 
                        "Content-Length: $D" "\r\n"
                        "Accept: Application/json" "\r\n"
                        "Content-Type: application/json" "\r\n"
                        "\r\n" 
                        "$H"),
                 PSTR("192.168.1.37"), stash.size(), sd); 

  session_id = ether.tcpSend(); 
}

void writeHeaders(BufferFiller& buf) {
  buf.print(F("HTTP/1.0 200 OK\r\nPragma: no-cache\r\n"));
}

void homePage(BufferFiller& buf) {
  writeHeaders(buf);
  buf.println(F("Content-Type: text/html\r\n"));
  buf.print(F(
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1' />"
    "<title>Cebada Mini</title>"
    "<link rel='stylesheet' href='//code.jquery.com/mobile/1.2.0/jquery.mobile-1.2.0.min.css' />"
    "<link rel='stylesheet' href='main.css' />"
    "<script src='//code.jquery.com/jquery-1.8.2.min.js'></script>"
    "<script src='//code.jquery.com/mobile/1.2.0/jquery.mobile-1.2.0.min.js'></script>"
//    "<script src='//stevenlevithan.com/assets/misc/date.format.js'></script>"
    "<script src='main.js'></script>"
    "</head><body><div id='main' data-role='page'>"
    "<div data-role='header' data-position='fixed'><h3>Cebada Mini</h3></div>"
    "<div data-role='content'>"
    "<ul id='list' data-role='listview' data-inset='true'></ul>"
    "<p id='info'></p></div></div></body></html>"));
}

void mainCss(BufferFiller& buf) {
  writeHeaders(buf);
  buf.println(F("Content-Type: text/css\r\n"));
  buf.print(F(
    ".ui-li-aside{font-weight:bold;font-size:xx-large;}"
    ".ui-li-aside > sup{font-size:large;}"
    "#info{margin-top:10px;text-align:center;font-size:small;}"));
}

void mainJs(BufferFiller& buf) {
  writeHeaders(buf);
  buf.println(F("Content-Type: application/javascript\r\n"));
  buf.print(F(
    "$(document).ready(function(){reload()});function reload(){$.getJSON('list.json',function(c)"
    "{var d=[];$.each(c.list,function(a,b){d.push('<li id=\"'+b.id+'\"><a><h3>'+b.name+'</h3>"
    "<p>'+b.id+'</p><p class=\"ui-li-aside\">'+b.val.toFixed(1)+'<sup>&deg;C</sup></p></a></li>')});"
    "$('#list').html(d.join('')).trigger('create').listview('refresh');var e=new Date();"
    "var uptime = e.getDate()+'/'+ (e.getMonth() + 1)+'/'+ e.getFullYear()+' '+ e.getHours()+':'+e.getMinutes()+':'+e.getSeconds();"
    "$('#info').html('Uptime: '+uptime+' ('+c.free+' bytes free)')});setTimeout(reload,10000)}"));
}

void listJson(BufferFiller& buf) {
  writeHeaders(buf);
  buf.println(F("Content-Type: application/json\r\n"));
  buf.print(F("{\"list\":["));
   float tp= getTempa();
  Serial.println(tp);
  int grados=(int)tp;
  int decimas=(tp-grados)*100;
    
  buf.emit_p(PSTR("{\"id\":\"ambiente\",\"name\":\"Sensor 1\",\"val\":$D.$D }"), grados, decimas);
  //buf.emit_p(PSTR("\"code\":\"1234\","));
  //buf.emit_p(PSTR("\"coldStatus\":true,"));
  //buf.emit_p(PSTR("\"heatStatus\":false,"));
  //buf.emit_p(PSTR("\"source\":\"1\","));
  //buf.emit_p(PSTR("\"temperature\":22,"));
  //buf.emit_p(PSTR("\"temperatureMax\":22,"));
  //buf.emit_p(PSTR("\"temperatureMin\":19,"));
  //buf.emit_p(PSTR("\"temperatureExt\":25"));
  buf.print(F("}"));
  buf.emit_p(PSTR("],\"uptime\":$L,\"free\":$D}"), millis(), freeRam());
}

boolean checkUrl(const __FlashStringHelper *val, const char* data) {
  const char PROGMEM *p = (const char PROGMEM *)val;
  while (1) {
    char c = pgm_read_byte(p++);
    if (c == 0) break;
    if (*data != c) return false;
    data++;
  }
  return true;
}
