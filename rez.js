let webSocket = new WebSocket("ws://localhost:8080/json");

webSocket.onmessage = function (event){
    let msg = JSON.parse(event.data);
    console.log(msg);
}

function addRow(){
    let empTab = document.getElementById('empTable');

    let rowCnt = empTab.rows.length;
    let tr = empTab.insertRow(rowCnt);
    tr = empTab.insertRow(rowCnt);

    for(let c = 0; c < arrHead.length; c++){
        let td = document.createElement('td');
        td = tr.insertCell(c);

        if(c === 0){
            let button = document.createElement('input');

            button.setAttribute('type', 'button');
            button.setAttribute('value', 'Remove');
            button.setAttribute('onclick', 'removeRow(this)');

            td.appendChild(button);
        }else{
            let ele = document.createElement('input');
            ele.setAttribute('type', 'text');
            ele.setAttribute('value', '');
            td.appendChild(ele);
        }
    }
}
function submit(){
    let myTab = document.getElementById('empTable');
    let arrValues = [];
    for(let row = 1; row < myTab.rows.length - 1; row++){
        for(let c = 0; c < myTab.rows[row].cells.length; c++){
            let element = myTab.rows.item(row).cells[c];
            if(element.childNodes[0].getAttribute('type') === 'text'){
                arrValues.push("'" + element.childNodes[0].value + "'");
            }
        }
    }
    document.getElementById('output').innerHTML = arrValues;
}