// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/fileNum',       //The server endpoint we are connecting to
        data: {
          info: ""
        },
        success: function (data) {
            loadFileLog(data);
            viewSVG(data.info[0].fileName); // first file
        },
        fail: function(error) {
            alert(error);
        }
    });

    // hidden objects
//    document.getElementById("addAttrform").style.display="none";
//    document.getElementById("showOtherAttr").style.display="none";

    // action listeners aka callback functions in order of appearance
    document.getElementById('viewFile').onclick = function () {
        let selectedVal = jQuery("#svg").children("option:selected").val();
        alert("User clicked on view file for " + selectedVal);
    };

    document.getElementById('titleform').onclick = function () {
        alert("user clicked edit for title");
        let role = jQuery(this).find('.editT');
        let value = jQuery(this).siblings(); // search DOM
        let values = value.text();

        let newTitle = value.find('.titleTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            jQuery(this).siblings().html('<input type="textbox" class="titleTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newTitle != "") {
                jQuery(this).siblings().html('<span id="titleText">' + newTitle + '</span>');
                role.html("Edit");
            }
            else {
                alert("Invalid, please fill the field");
            }
        }
    };

    document.getElementById('descform').onclick = function () {
        alert("user clicked edit for description");
        let role = jQuery(this).find('.editD');
        let value = jQuery(this).siblings(); // search DOM
        let values = value.text();

        let newDesc = value.find('.descTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            jQuery(this).siblings().html('<input type="textbox" class="descTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newDesc != "") {
                jQuery(this).siblings().html('<span id="descText">' + newDesc + '</span>');
                role.html("Edit");
            }
            else {
                alert("Invalid, please fill the field");
            }
        }
    };

/*
    document.getElementById('viewAttr').onclick = function () {
        document.getElementById("showOtherAttr").style.display="block";
    };

    document.getElementById('attrform').onclick = function () {
        alert("user clicked edit for attribute");
        let role = jQuery(this).find('.editA');
        let value = jQuery(this).siblings(); // search DOM
        let values = value.text();

        let newAttr = value.find('.attrTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            jQuery(this).siblings().html('<input type="textbox" class="attrTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newAttr != "") {
                jQuery(this).siblings().html('<span id="attrText">' + newAttr + '</span>');
                role.html("Edit");
            }
            else {
                alert("Invalid, please fill the field");
            }
        }
    };

    document.getElementById('aAttr').onclick = function () {
        document.getElementById("addAttrform").style.display="block";
        let newName = document.getElementById('nameAttr').value;
        let newValue = document.getElementById('valueAttr').value;
        document.getElementById('attrSubmit').onclick = function (){
            alert("Submitted this element to add to other attributes: " + newName + " " + newValue);
        };
    };
*/
    document.getElementById('sRect').onclick = function () {
        let newScale = document.getElementById('scaleValue').value;
        alert("Submitted scaling rectangles by: " + newScale);
    };

    document.getElementById('sCirc').onclick = function () {
        let newScale = document.getElementById('scaleValue').value;
        alert("Submitted scaling circles by: " + newScale);
    };

    document.getElementById('createNewSVG').onclick = function () {
        alert("User clicked on create a new SVG");
    };

    document.getElementById('addShape').onclick = function () {
        let selectedVal = jQuery("#svgShape").children("option:selected").val();
        alert("User clicked on add a shape for " + selectedVal);
    };

});

// This function will fill in the file log panel and the drop down menu
function loadFileLog(data){

    // create the drop down menus for both svg view panel and funcitonality panel
    let selectionSV = "<select id=\"svg\"></select>";
    jQuery("#svgFiles").append(selectionSV);

    let selectionF = "<select id=\"svgShape\"></select>";
    jQuery("#svgFilesAddShape").append(selectionF);

    // if no files, display no files
    if (data.info.length == 0){
        let newRow = "<tr><td colspan=\"7\">No files</td></tr>";
        jQuery("#fileLog").append(newRow);
    }
    else{

        // for each valid file ...
        for (let i = 0; i < data.info.length; ++i) {

            // a. get the file name without the path
            let file = data.info[i].fileName.split('/').pop();

            // b. place in the drop down menus
            let newOption = "<option value=\"" + file + "\">" + file + "</option>";
            jQuery("#svg").append(newOption);
            jQuery("#svgShape").append(newOption);

            // c. place in a row in a table
            let newRow = "<tr><td><a href=\"" + data.info[i].fileName + "\" download><img src=\"" + data.info[i].fileName + "\" class=\"logImage\" /></a></td><td><a href=\"" + data.info[i].fileName + "\" download>" + file + "</a></td><td>" + data.info[i].fileSize + "KB</td><td>" + data.info[i].numbers.numRect + "</td><td>" + data.info[i].numbers.numCirc + "</td><td>" + data.info[i].numbers.numPaths + "</td><td>" + data.info[i].numbers.numGroups + "</td></tr>";
            jQuery("#fileLog").append(newRow);
        }
    }

}

function viewSVG(fileName){

    let newFile = "uploads/rects.svg";

    // 1. ajax call to get the file information
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/fileInfo',       //The server endpoint we are connecting to
        data: {
          info: newFile
        },
        success: function (data) {
            // 1. place a table
            console.log(data.info.title);
            console.log(data.info.description);

            // to access:
            let index = 1;
            for (let i of data.info.rectangles){

                let data = "x = " + i.x + i.units + " y = " + i.y + i.units + " width = " + i.w + i.units + " height = " + i.h + i.units;
                console.log(data);
                let otherAttrNum = i.numAttr;

                let newRow = "<tr><td>Rectangle " + index + "</td><td>" + data + "</td><td>" + otherAttrNum + "<br><button id=\"viewAttrP\" class=\"btn btn-secondary\">View</button><button id=\"aAttrP\" class=\"btn btn-secondary\">Add</button><div id=\"showOtherAttrP\"><span id=\"attrTextP\"></span><a id=\"attrformP\" href=\"#\"><span class=\"editA\">Edit</span></a></div><br></td></tr>";
                jQuery("#fileView").append(newRow);
/*
                                    <form ref='attrform' id='addAttrform'>
                                        <div class="form-group">
                                            <br>
                                            <input type="text" class="form-control" id="nameAttr" value="Name of new Attribute" placeholder="Placeholder">
                                            <input type="text" class="form-control" id="valueAttr" value="Value of new Attribute" placeholder="Placeholder">
                                            <br>
                                            <input type='submit' class="btn btn-secondary" id="attrSubmit">
                                        </div>
                                    </form>
*/
                ++index;

            }
        },
        fail: function(error) {
            alert(error);
        }
    });


}
