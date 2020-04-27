//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

function loadJSON_001(file_path) {   

    var xobj = new XMLHttpRequest();
        xobj.overrideMimeType("application/json");
    xobj.open('GET', file_path, false); 
	xobj.send(null);  
    
          if (xobj.status == "200") {
            return xobj.responseText;
         }
    
 }
 
function read_json(file_path) 
 {
  // Parse JSON string into object
  var response = loadJSON_001(file_path) ;
  console.log(response)
   var actual_JSON = JSON.parse(response );
	
    console.log(actual_JSON.length)
 }