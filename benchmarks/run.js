var fs = require("fs");
var path = require("path");
var test = require("assert");

var ntpl = require("../lib/nTPL/nTPL").plugins("ntpl.block", "ntpl.filter").ntpl;

var tests = [];

for (var i = 1, filename; path.existsSync( (filename = "./tests/test-" + i) + ".js"); i++)
	tests[i] = require(filename);	

var total = 0, q = 0;
console.log("Start measuring");
var start = +new Date, t =0;
for (var i=1; i<1E8;i++) {
	t = 10 * 20 * 30;
}

q = (+new Date - start);
console.log("q=" + q);

for (var i = 1, len = tests.length; i<len;i++) {
	start = +new Date;
	tests[i].run(ntpl);
	total += +new Date - start;
}
	
console.log("Benchmark result: " + (Math.round(1E4* q/total)/ 1E2));