let webSocket = new WebSocket("ws://localhost:8080");

let switcher = 0;

const ping = '{ "__MESSAGE__":"message", "input":"ping"}';
const rbusy = '{ "__MESSAGE__":"message", "input":"rbusy"}';
const obj = '{ "__MESSAGE__":"message", "input":"json"}';
//const obj2 = '{ "__MESSAGE__":"message", "input":"close"}';
//const obj3 = '{ "__MESSAGE__":"message", "input":"insert", "name":"alma","class":"I","place":"Itt","type":"1","current":"20","cprotect":"1","good":"1","resistance":"20","final":"ottbasszameg"}';
//const obj4 = '{ "__MESSAGE__":"message", "input":"delete", "id":"1"}';
const base = '{ "__MESSAGE__":"message", "input":"insert",';
let gArray = [];
let gData = [];
let busy = false;
let done = false;
let plength = 0;
let iVal;

let dRow = -1;

function opener(event){
	switcher = 1;
	console.log("connected");
	setInterval(() => {
		webSocket.send(ping);
	}, 5000);
}


function closer(event){
	switcher = -1;
}
function update(){
	webSocket.send(obj);
}

function messenger(event){
    	let msg = JSON.parse(event.data);
	if(msg["input"] === "pong"){
		if(gArray.length !== 0){
			let message = base + '"eplace":"'+gArray[0][0] + '","name":"'+ gArray[0][1]+ '","place":"'+gArray[0][2]+'","class":"'+gArray[0][3]+'","type":"'+gArray[0][4]+'","current":"'+gArray[0][5]+'","cprotect":"'+gArray[0][6]+'","good":"'+gArray[0][7]+'","resistance":"'+gArray[0][8]+'","final":"'+gArray[0][9]+'"}';
			gArray.shift();
			webSocket.send(message);
	    	}
	}else if(msg["input"] === "data"){
		gData = msg["reports"];
		if(gData != null){
			addRow();
		}
		setTimeout(function(){
			done = false;
		}, 5000);
		
	}else if(msg["input"] === "nbusy"){
		busy = false;
	}else if(msg["input"] === "busy"){
		busy = true;
		
		setTimeout(function(){
			webSocket.send(rbusy);
		}, 5000);
	}
	
	if(busy === false){
		if(done === false){
			update();
			done = true;
		}
	}
}

iVal = setInterval(() => {
	if(switcher === -1){
		webSocket = new WebSocket("ws://localhost:8080");
		setTimeout(function(){
			if(webSocket.readyState === 1){
				webSocket.onopen = opener;
				webSocket.onclose = closer;
				webSocket.onmessage = messenger;
			}
		}, 3000);
	}
}, 5000);

webSocket.onopen = opener;
webSocket.onclose = closer;
webSocket.onmessage = messenger;


function addRow(){
	let size = gData.length;

	plength = document.getElementById('empTable').rows.length;

	if(dRow !== -1){
		document.getElementById("empTable").deleteRow(dRow);
		dRow = -1;
	}
	
	if(plength === document.getElementById('empTable').rows.length){
		setTimeout(function(){ 
			location.reload();
		}, 5000);
		
	}
	
	if(gData[gData.length-1] == null){
		size--;
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
						let tRowCnt = rowCnt-2;
						cell.innerHTML = tRowCnt.toString();
					}else if(c === 2){
						cell.innerHTML = '<div id = "' + gData[i]["name"] + '">' + gData[i]["eplace"] + ":"+ gData[i]["name"] + '</div>';
					}else if(c === 3){
						cell.innerHTML = gData[i]["place"];
					}else if(c === 4){
						let tmp;
						
						if(gData[i]["class"] === "one"){
							tmp = "I";
						}else if(gData[i]["class"] === "two"){
							tmp = "II";
						}else if(gData[i]["class"] === "three"){
							tmp = "III";
						}else{
							tmp = gData[i]["class"]
						}
						cell.innerHTML = tmp;
					}else if(c === 5){
						let tmp;
						if(gData[i]["type"] === 1){
							tmp = "B";
						}else if(gData[i]["type"] === 2){
							tmp = "C";
						}else if(gData[i]["type"] === 3){
							tmp = "GL";
						}else if(gData[i]["type"] === 4){
							tmp = "GG";
						}
					
						cell.innerHTML = tmp;
					}else if(c === 6){
						cell.innerHTML = gData[i]["current"];
					}else if(c === 7){
						let tmp;
						if(gData[i]["cprotect"] !== 0){
							tmp = "van";
						}else{
							tmp = "nincs";
						}
						cell.innerHTML = tmp;
					}else if(c === 8){
						let tmp
						if(gData[i]["good"] === 0){
							tmp = "NF";
						}else{
							tmp = "MF";
						}
						cell.innerHTML = tmp;
					}else if(c === 9){
						cell.innerHTML = gData[i]["resistance"];
					}else if(c === 10){
						cell.innerHTML = gData[i]["final"];
					}else if(c === 11){
						cell.innerHTML = '<button onclick="copy(' + rowCnt + ')">Másolás</button>'
					}
				}
			}
		}
	
	}
}

function removeRow(id, id2){
	let tmp = '{ "__MESSAGE__":"message", "input":"delete", "id":"' + id +'"}';
	dRow = id2;
	webSocket.send(tmp);
}

function submit(){
    let arrValues = [];
    let tmp;
    
    tmp = document.getElementById("name").value;
    tmp = tmp.substring(0, tmp.indexOf(':'));
    arrValues.push(tmp);
    
    tmp = document.getElementById("name").value;
    tmp = tmp.split(":").pop();
    arrValues.push(tmp);
    
    
    arrValues.push(document.getElementById("place").value);
    arrValues.push(document.getElementById("class").value);
    if(document.getElementById("type").value === "B"){
    	tmp = "1";
    }else if(document.getElementById("type").value === "C"){
	tmp = "2";
    }else if(document.getElementById("type").value === "GL"){
	tmp = "3";
    }else if(document.getElementById("type").value === "GG"){
	tmp = "4";
    }else{
	tmp = "0";
    }
    
    arrValues.push(tmp);
    arrValues.push(document.getElementById("current").value);

    if(document.getElementById("cprotect").value === "van"){
    	tmp = 1;
    }else{
    	tmp = 0;
    }

    arrValues.push(tmp);
    
    if(document.getElementById("good").value === "mf"){
    	tmp = 1;
    }else{
    	tmp = 0;
    }

    arrValues.push(tmp);
    tmp = document.getElementById("resistance").value;
    
    if(tmp === ""){
    	tmp = 0;
    }else if(document.getElementById("resistance").value === "PE-folyt"){
    	tmp = null;
    	do{
    		tmp = prompt("Ellenállás értéke", 20);
    	}while(isNaN(tmp) || tmp === "");
    }
    arrValues.push(tmp);
    arrValues.push(document.getElementById("final").value);
    
    gArray.push(arrValues);
    
    if(webSocket.readyState === 1){
    	webSocket.send(rbusy);
    }
    
}

function copy(id){
	let empTab = document.getElementById('empTable');
	
	for(let i = 2; i < empTab.rows[id].cells.length-3; i++){
		if(i === 2){
			document.getElementById("name").defaultValue = empTab.rows[id].cells[i].innerText;
		}else if(i === 3){
			document.getElementById("place").defaultValue = empTab.rows[id].cells[i].innerText;
		}else if(i === 4){
			changeContent("class",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 5){
			changeContent("type",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 6){
			changeContent("current",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 7){
			changeContent("cprotect",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 8){
			changeContent("good",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 9){
			changeContent("resistance",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}else if(i === 10){
			changeContent("final",empTab.rows[id].cells[i].innerText,empTab.rows[id].cells[i].innerText);
		}
	}
}

function changeContent(id,newv,newt){
     let opt = document.getElementById(id).options[0];
     opt.value =  newv;
     opt.text = newt;
}
