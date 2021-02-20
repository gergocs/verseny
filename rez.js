let webSocket = new WebSocket("ws://localhost:8080");

let switcher = 0;

let obj = '{ "__MESSAGE__":"message", "input":"json"}';
let obj2 = '{ "__MESSAGE__":"message", "input":"close"}';
let obj3 = '{ "__MESSAGE__":"message", "input":"insert", "name":"alma","class":"I","place":"Itt","type":"1","current":"20","cprotect":"1","good":"1","resistance":"20","final":"ottbasszameg"}';
let obj4 = '{ "__MESSAGE__":"message", "input":"delete", "id":"1"}';
const base = '{ "__MESSAGE__":"message", "input":"insert",';

let gArray = [];

let gData = [];

let counter = 0;

webSocket.onopen = function(event){
	switcher = 1;
	update();
	//webSocket.send(obj4);
}

webSocket.onclose = function(event){
	switcher = -1;
}

webSocket.onerror = function(event){
	switcher = -1;
}

webSocket.onmessage = function (event){
    let msg = JSON.parse(event.data);
    setTimeout(function(){
	    if(msg["input"] == "ready"){
	    		if(gArray.length != 0){
	    		
			let message = base + '"name":"'+gArray[0][0]+'","class":"'+gArray[0][2]+'","place":"'+gArray[0][1]+'","type":"'+gArray[0][3]+'","current":"'+gArray[0][4]+'","cprotect":"'+gArray[0][5]+'","good":"'+gArray[0][6]+'","resistance":"'+gArray[0][7]+'","final":"'+gArray[0][8]+'"}';
				gArray.shift();
				
				setTimeout(function(){
					webSocket.send(obj);
				}, 5000);
				webSocket.send(message);
	    		}
	    }else if(msg["input"] == "data"){
	    	gData = msg["reports"];
	    	if(gData != null){
	    		addRow();
	    	}
	    }
    
    }, 2000);
    
}

function update(){
	webSocket.send(obj);
}

function addRow(){
	let size = gData.length;

	if(gData[1] == null){
		size = size-1;
	}

	for(let i = 0; i < size; i++){
		if(document.getElementById(gData[i]["name"]) == null){
			let empTab = document.getElementById('empTable');

			    let rowCnt = empTab.rows.length;
			    
			    let tr = empTab.insertRow(rowCnt);

			    for(let c = 0; c < empTab.rows[0].cells.length+empTab.rows[1].cells.length; c++){
				let cell = tr.insertCell(c);
				if(c === 0){
				    cell.innerHTML = '<button onclick="removeRow('+ gData[i]["id"] + ',' + rowCnt + ')">-</button>';
				    
				}else{
					if(c === 1){
						cell.innerHTML = rowCnt-2;
					}else if(c === 2){
						cell.innerHTML = '<div id = "' + gData[i]["name"] + '">' + gData[i]["name"] + '</div>';
					}else if(c === 3){
						cell.innerHTML = gData[i]["place"];
					}else if(c === 4){
						let tmp;
						
						if(gData[i]["class"] == "one"){
							tmp = "I";
						}else if(gData[i]["class"] == "two"){
							tmp = "II";
						}else if(gData[i]["class"] == "three"){
							tmp = "III";
						}else{
							tmp = gData[i]["class"]
						}
						cell.innerHTML = tmp;
					}else if(c === 5){
						let tmp;
						if(gData[i]["type"] == 1){
							tmp = "B";
						}else if(gData[i]["type"] == 2){
							tmp = "C";
						}else if(gData[i]["type"] == 3){
							tmp = "GL";
						}else if(gData[i]["type"] == 4){
							tmp = "GG";
						}
					
						cell.innerHTML = tmp;
					}else if(c === 6){
						cell.innerHTML = gData[i]["current"];
					}else if(c === 7){
						let tmp;
						if(gData[i]["cprotect"] != 0){
							tmp = "van";
						}else{
							tmp = "nincs";
						}
						cell.innerHTML = tmp;
					}else if(c === 8){
						let tmp
						if(gData[i]["good"] == 0){
							tmp = "NF";
						}else{
							tmp = "MF";
						}
						cell.innerHTML = tmp;
					}else if(c === 9){
						cell.innerHTML = gData[i]["resistance"];
					}else if(c === 10){
						cell.innerHTML = gData[i]["final"];
					}
				}
			}
		}
	
	}
}

function removeRow(id, id2){
	
	
	let tmp = '{ "__MESSAGE__":"message", "input":"delete", "id":"' + id +'"}';
	setTimeout(function(){
		webSocket.send(obj);
		document.getElementById("empTable").deleteRow(id2);
	}, 10000);
	webSocket.send(tmp);
}

function submit(){
    let arrValues = [];
    let tmp;
    
    arrValues.push(document.getElementById("name").value);
    arrValues.push(document.getElementById("place").value);
    arrValues.push(document.getElementById("class").value);
    if(document.getElementById("type").value == "B"){
    	tmp = "1";
    }else if(document.getElementById("type").value == "C"){
	tmp = "2";
    }else if(document.getElementById("type").value == "GL"){
	tmp = "3";
    }else if(document.getElementById("type").value == "GG"){
	tmp = "4";
    }else{
	tmp = "0";
    }
    
    arrValues.push(tmp);
    arrValues.push(document.getElementById("current").value);

    if(document.getElementById("cprotect").value == "van"){
    	tmp = 1;
    }else{
    	tmp = 0;
    }

    arrValues.push(tmp);
    
    if(document.getElementById("good").value == "mf"){
    	tmp = 1;
    }else{
    	tmp = 0;
    }

    arrValues.push(tmp);
    arrValues.push(tmp); //TODO
    arrValues.push(document.getElementById("final").value);
    
    gArray.push(arrValues);
}
