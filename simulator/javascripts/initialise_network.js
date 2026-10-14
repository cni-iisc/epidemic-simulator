//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

// JSON input loading.
//
// The simulation code is one long synchronous pipeline, so it needs its input
// files available synchronously. Instead of blocking the main thread with a
// synchronous XMLHttpRequest (deprecated), the files are downloaded up front
// with fetch() -- asynchronously -- into JSON_CACHE, and loadJSON() simply
// reads from that cache.
//
//   preloadJSON(paths, optionalPaths)  -> Promise, resolves once all are cached
//   loadJSON(path)                     -> cached response text (synchronous)
//   clearJSONCache(keepPrefix)         -> drop cached files (e.g. other cities)

var JSON_CACHE = {};      // path -> response text
var JSON_PENDING = {};    // path -> in-flight Promise

function fetchJSONText(file_path) {
    if (Object.prototype.hasOwnProperty.call(JSON_CACHE, file_path)) {
        return Promise.resolve(JSON_CACHE[file_path]);
    }
    if (JSON_PENDING[file_path]) {
        return JSON_PENDING[file_path];
    }
    var request = fetch(file_path).then(function (response) {
        if (!response.ok) {
            throw new Error('HTTP ' + response.status + ' while loading ' + file_path);
        }
        return response.text();
    }).then(function (text) {
        JSON_CACHE[file_path] = text;
        delete JSON_PENDING[file_path];
        return text;
    }, function (err) {
        delete JSON_PENDING[file_path];
        throw err;
    });
    JSON_PENDING[file_path] = request;
    return request;
}

// Load every path in `paths` (rejects if any fails) and every path in
// `optionalPaths` (failures ignored; loadJSON() reports them only if used).
function preloadJSON(paths, optionalPaths) {
    var required = paths.map(fetchJSONText);
    var optional = (optionalPaths || []).map(function (p) {
        return fetchJSONText(p).catch(function () { return null; });
    });
    return Promise.all(required.concat(optional));
}

// Synchronous read from the cache. Never touches the network.
function loadJSON(file_path) {
    if (Object.prototype.hasOwnProperty.call(JSON_CACHE, file_path)) {
        return JSON_CACHE[file_path];
    }
    throw new Error('JSON file was not preloaded: ' + file_path +
        ' (call preloadJSON() before using it)');
}

// Drop cached files so memory does not pile up when switching city.
// Files whose path starts with `keepPrefix` are kept.
function clearJSONCache(keepPrefix) {
    Object.keys(JSON_CACHE).forEach(function (p) {
        if (!keepPrefix || p.indexOf(keepPrefix) !== 0) {
            delete JSON_CACHE[p];
        }
    });
}

function read_json(file_path) {
    return fetchJSONText(file_path).then(function (response) {
        var actual_JSON = JSON.parse(response);
        console.log(actual_JSON.length);
        return actual_JSON;
    });
}
