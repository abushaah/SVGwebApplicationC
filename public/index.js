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

    // action listeners aka callback functions in order of appearance
    document.getElementById('viewFile').onclick = function () {
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        viewSVG(fileName);
    };

    document.getElementById('editTitle').onclick = function () {
        let newValue = document.getElementById('newTitleDesc').value;
        // enforce restrictions
        if (newValue.length > 256){
            alert(newValue + " is greater than 256 characters, the length restriction");
        }
        else{
            // make an request to change the title
            let selectedVal = jQuery("#svg").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/editTitle',
                data: {
                  info: fileName,
                  newValue: newValue
                },
                success: function (data) {
                    console.log("change successful");
                },
                fail: function(error) {
                    alert(error);
                }
            });
        }
    };

    document.getElementById('editDescr').onclick = function () {
        let newValue = document.getElementById('newTitleDesc').value;
        if (newValue.length > 256){
            alert(newValue + " is greater than 256 characters, the length restriction");
        }
        else{
            // make an request to change the title
            let selectedVal = jQuery("#svg").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/editDesc',
                data: {
                  info: fileName,
                  newValue: newValue
                },
                success: function (data) {
                    console.log("change successful");
                },
                fail: function(error) {
                    alert(error);
                }
            });
        }
    };

    document.getElementById('sRect').onclick = function () {
        let newScale = document.getElementById('scaleValue').value;
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/scaleRects',
            data: {
              info: fileName,
              newScale: newScale
            },
            success: function (data) {
                console.log("change successful");
            },
            fail: function(error) {
                alert(error);
            }
        });
    };

    document.getElementById('sCirc').onclick = function () {
        let newScale = document.getElementById('scaleValue').value;
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/scaleCircs',
            data: {
              info: fileName,
              newScale: newScale
            },
            success: function (data) {
                console.log("change successful");
            },
            fail: function(error) {
                alert(error);
            }
        });
    };

    document.getElementById('viewAttr').onclick = function () {
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        let component = (jQuery("#components").children("option:selected").text()).match(/[a-zA-Z]+/g); // regex from online
        let componentNumber = (jQuery("#components").children("option:selected").text()).match(/\d+/g);
    };

    document.getElementById('addAttr').onclick = function () {
        let newName = document.getElementById('nameAttr').value;
        let newValue = document.getElementById('valueAttr').value;
        alert("Submitted this element to add to other attributes: " + newName + " " + newValue);
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

    // 1. create the drop down menus for both svg view panel and funcitonality panel
    let selectionSV = "<select id=\"svg\"></select>";
    jQuery("#svgFiles").append(selectionSV);

    let selectionF = "<select id=\"svgShape\"></select>";
    jQuery("#svgFilesAddShape").append(selectionF);

    // 2. if no files, display no files
    if (data.info.length == 0){
        let newRow = "<tr><td colspan=\"7\">No files</td></tr>";
        jQuery("#fileLog").append(newRow);
    }
    else{

        // 3. for each valid file ...
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

    jQuery("#fileChange").html("Edit " + fileName.split('/').pop());

    // 1. ajax call to get the file information
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/fileInfo',       //The server endpoint we are connecting to
        data: {
          info: fileName
        },
        success: function (data) {
            // 2. place a table

            // a. image, title description
            let imageString = "<img src=\"" + fileName + "\" class=\"svgImage\"/>";
            jQuery("#svgViewImg").html(imageString);
            jQuery("#titleText").html(data.info.title);
            jQuery("#descText").html(data.info.description);
            let table = "";

            // b. components to access
            let rectIndex = 1;
            for (let i of data.info.rectangles){
                let data = "x = " + i.x + i.units + " y = " + i.y + i.units + " width = " + i.w + i.units + " height = " + i.h + i.units;
                let otherAttrNum = i.numAttr;
                let newRow = "<tr><td>Rectangle " + rectIndex + "</td><td>" + data + "</td><td>" + otherAttrNum + "</td></tr>";
                table = table + newRow;
                ++rectIndex;

            }
            let circIndex = 1;
            for (let i of data.info.circles){
                let data = "x = " + i.cx + i.units + " y = " + i.cy + i.units + " r = " + i.r + i.units;
                let otherAttrNum = i.numAttr;
                let newRow = "<tr><td>Circle " + circIndex + "</td><td>" + data + "</td><td>" + otherAttrNum + "</td></tr>";
                table = table + newRow;
                ++circIndex;
            }
            let pathIndex = 1;
            for (let i of data.info.paths){
                let data = "path data = " + i.d;
                let otherAttrNum = i.numAttr;
                let newRow = "<tr><td>Path " + pathIndex + "</td><td>" + data + "</td><td>" + otherAttrNum + "</td></tr>";
                table = table + newRow;
                ++pathIndex;
            }
            let groupIndex = 1;
            for (let i of data.info.groups){
                let data = i.children + " child elements";
                let otherAttrNum = i.numAttr;
                let newRow = "<tr><td>Group " + groupIndex + "</td><td>" + data + "</td><td>" + otherAttrNum + "</td></tr>";
                table = table + newRow;
                ++groupIndex;
            }

            jQuery("#dynamicBodySVG").html(table);

            // 3. for the functionality part of other attributes
            let selectionOA = "<label for=\"componentLabel\">Choose a component: </label><select id=\"components\"></select>";
            jQuery("#viewOtherAttr").html(selectionOA);
            for (let i = 1; i < rectIndex; ++i){
                let newOption = "<option value=\"rect" + i + "\">Rectangle " + i + "</option>";
                jQuery("#components").append(newOption);
            }
            for (let i = 1; i < circIndex; ++i){
                let newOption = "<option value=\"circ" + i + "\">Circle " + i + "</option>";
                jQuery("#components").append(newOption);
            }
            for (let i = 1; i < pathIndex; ++i){
                let newOption = "<option value=\"path" + i + "\">Path " + i + "</option>";
                jQuery("#components").append(newOption);
            }
            for (let i = 1; i < groupIndex; ++i){
                let newOption = "<option value=\"group" + i + "\">Group " + i + "</option>";
                jQuery("#components").append(newOption);
            }

        },
        fail: function(error) {
            alert(error);
        }
    });


}
