function addRow() {
    let empTab = document.getElementById('empTable');

    let rowCnt = empTab.rows.length;   // table row count.
    let tr = empTab.insertRow(rowCnt); // the table row.
    tr = empTab.insertRow(rowCnt);

    for (let c = 0; c < arrHead.length; c++) {
        let td = document.createElement('td'); // table definition.
        td = tr.insertCell(c);

        if (c === 0) {      // the first column.
            // add a button in every new row in the first column.
            let button = document.createElement('input');

            // set input attributes.
            button.setAttribute('type', 'button');
            button.setAttribute('value', 'Remove');

            // add button's 'onclick' event.
            button.setAttribute('onclick', 'removeRow(this)');

            td.appendChild(button);
        }
        else {
            // 2nd, 3rd and 4th column, will have textbox.
            let ele = document.createElement('input');
            ele.setAttribute('type', 'text');
            ele.setAttribute('value', '');

            td.appendChild(ele);
        }
    }
}
function submit() {
    let myTab = document.getElementById('empTable');
    let arrValues = [];

    // loop through each row of the table.
    for (let row = 1; row < myTab.rows.length - 1; row++) {
        // loop through each cell in a row.
        for (let c = 0; c < myTab.rows[row].cells.length; c++) {
            let element = myTab.rows.item(row).cells[c];
            if (element.childNodes[0].getAttribute('type') === 'text') {
                arrValues.push("'" + element.childNodes[0].value + "'");
            }
        }
    }

    // The final output.
    document.getElementById('output').innerHTML = arrValues;
    //console.log (arrValues);   // you can see the array values in your browsers console window. Thanks :-)
}