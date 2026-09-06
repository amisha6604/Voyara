// server/src/engineBridge.js
//
// Talks to the C++ engine via stdin/stdout (spec Section 15/20.3 — simplest
// MVP integration, no native bindings, no HTTP service yet).
//
// Contract (you'll implement the C++ side of this in engine/src/main.cpp,
// once dijkstra.cpp actually works):
//
//   Node writes ONE line of JSON to the engine's stdin, e.g.:
//     {"command":"shortest_path","origin":"VNS","destination":"DEL","weightType":"PRICE"}
//
//   Engine writes ONE line of JSON to stdout, e.g.:
//     {"found":true,"totalCost":3900,"path":["VNS","DEL"],"flightIds":["FL0002"],
//      "nodesExplored":4,"runtimeMs":0.12}
//
//   Then the engine process exits (one request per process invocation, for
//   now — simplest possible contract. A persistent long-running engine
//   process is a later optimization, not needed yet).

const { spawn } = require('child_process');
const path = require('path');

const ENGINE_PATH = path.join(__dirname, '..', '..', 'engine', 'engine');
// ^ this is the compiled binary produced by `make` in engine/

function callEngine(requestObj, timeoutMs = 5000) {
  return new Promise((resolve, reject) => {
    const proc = spawn(ENGINE_PATH);
    let stdout = '';
    let stderr = '';
    let settled = false;

    const timer = setTimeout(() => {
      if (!settled) {
        settled = true;
        proc.kill();
        reject(new Error(`Engine call timed out after ${timeoutMs}ms`));
      }
    }, timeoutMs);

    proc.stdout.on('data', (chunk) => { stdout += chunk.toString(); });
    proc.stderr.on('data', (chunk) => { stderr += chunk.toString(); });

    proc.on('error', (err) => {
      if (!settled) {
        settled = true;
        clearTimeout(timer);
        reject(new Error(`Failed to spawn engine: ${err.message}`));
      }
    });

    proc.on('close', (code) => {
      if (settled) return;
      settled = true;
      clearTimeout(timer);
      if (code !== 0) {
        reject(new Error(`Engine exited with code ${code}. stderr: ${stderr}`));
        return;
      }
      try {
        const result = JSON.parse(stdout.trim());
        resolve(result);
      } catch (err) {
        reject(new Error(`Engine returned invalid JSON: ${stdout}`));
      }
    });

    proc.stdin.write(JSON.stringify(requestObj) + '\n');
    proc.stdin.end();
  });
}

module.exports = { callEngine };
