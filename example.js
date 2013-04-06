var ntpl = require("./lib/nTPL/nTPL").plugins('ntpl.block', 'ntpl.filter').ntpl;

var base = ntpl({
  template: "./tpl/base.html",
  watch: true
});
	
var home = ntpl({
  template: "./tpl/index.html",
  watch: true
});
	
var http = require('http');

var server = http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end(home({
      username: "Paul",
      userfax: "12345678",
      usermail: "a@a.com"
    })); 
}).listen(8000, "127.0.0.1", function() {
	var addr = server.address();
	console.log('Server running at http://' + addr.address + ':' + addr.port);
});