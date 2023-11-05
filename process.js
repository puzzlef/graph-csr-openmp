const fs = require('fs');
const os = require('os');
const path = require('path');

const RGRAPH = /^Finding byte sum of file .*\/(.*?)\.mtx \.\.\./m;
const RRESLT = /^\{adv=(.+?), block=(.+?), mode=(.+?)\} -> {(.+?)ms, sum=(.+?)} (.+)/m;
const MODES  = [
  'default',
  'madvice',
  'mmap',
];




// *-FILE
// ------

function readFile(pth) {
  var d = fs.readFileSync(pth, 'utf8');
  return d.replace(/\r?\n/g, '\n');
}

function writeFile(pth, d) {
  d = d.replace(/\r?\n/g, os.EOL);
  fs.writeFileSync(pth, d);
}




// *-CSV
// -----

function writeCsv(pth, rows) {
  var cols = Object.keys(rows[0]);
  var a = cols.join()+'\n';
  for (var r of rows)
    a += [...Object.values(r)].map(v => `"${v}"`).join()+'\n';
  writeFile(pth, a);
}




// *-LOG
// -----

function readLogLine(ln, data, state) {
  ln = ln.replace(/^\d+-\d+-\d+ \d+:\d+:\d+\s+/, '');
  if (RGRAPH.test(ln)) {
    var [, graph] = RGRAPH.exec(ln);
    if (!data.has(graph)) data.set(graph, []);
    state = {graph};
  }
  else if (RRESLT.test(ln)) {
    var [, early_madvice, block_size, mode, time, sum, technique] = RRESLT.exec(ln);
    data.get(state.graph).push(Object.assign({}, state, {
      early_madvice: parseFloat(early_madvice)? true : false,
      block_size:    parseFloat(block_size),
      mode:          MODES[parseFloat(mode)],
      time:          parseFloat(time),
      sum:           parseFloat(sum),
      technique:     technique,
    }));
  }
  return state;
}

function readLog(pth) {
  var text  = readFile(pth);
  var lines = text.split('\n');
  var data  = new Map();
  var state = null;
  for (var ln of lines)
    state = readLogLine(ln, data, state);
  return data;
}




// PROCESS-*
// ---------

function processCsv(data) {
  var a = [];
  for (var rows of data.values())
    a.push(...rows);
  return a;
}




// RANDOM-*
// --------

function writeRandomInts(pth, n, min, max) {
  var a = '';
  for (var i=0; i<n; i++) {
    var f = Math.random();
    var r = Math.floor(f * (max-min+1) + min);
    a += r+'\n';
  }
  writeFile(pth, a);
}




// MAIN
// ----

function main(cmd, log, out) {
  if (path.extname(out)==='') cmd += '-dir';
  switch (cmd) {
    case 'csv':
      var data = readLog(log);
      var rows = processCsv(data);
      writeCsv(out, rows);
      break;
    case 'csv-dir':
      var data = readLog(log);
      for (var [graph, rows] of data)
        writeCsv(path.join(out, graph+'.csv'), rows);
      break;
    case 'random-ints':
      var [n, min, max] = log.split(',').map(v => parseFloat(v.trim()));
      writeRandomInts(out, n, min, max);
      break;
    default:
      console.error(`error: "${cmd}"?`);
      break;
  }
}
main(...process.argv.slice(2));
