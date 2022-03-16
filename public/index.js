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
            if (data.info == null){
                alert("Error on ready, files not generated");
                console.log("Error on ready, files not generated");
            }
            else{
                loadFileLog(data);
                viewSVG(data.info[0].fileName); // first file
            }
        },
        fail: function(error) {
            alert(error);
        }
    });

    // action listeners aka callback functions in order of appearance
    document.getElementById('viewFile').onclick = function () {
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        jQuery("#showAttributes").html(""); // reset status
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
                    if (data.info == false){
                        console.log("Edit for title was not successful, no changes made to file");
                        alert("Change not successful");
                    }
                    else{
                        alert("Change successful");
                    }
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
                    if (data.info == false){
                        console.log("Edit for description was not successful, no changes made to file");
                        alert("Change not successful");
                    }
                    else{
                        alert("Change successful");
                    }
                },
                fail: function(error) {
                    alert(error);
                }
            });
        }
    };

    document.getElementById('sRect').onclick = function () {
        let newScale = parseFloat(document.getElementById('scaleValue').value);
        if (isNaN(newScale)){
            alert("Not a number");
            return;
        }
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
                if (data.info == false){
                    console.log("Scaling rectangles was not successful, no changes made to file");
                    alert("Change not successful");
                }
                else{
                    alert("Change successful");
                }
            },
            fail: function(error) {
                alert(error);
            }
        });
    };

    document.getElementById('sCirc').onclick = function () {
        let newScale = parseFloat(document.getElementById('scaleValue').value);
        if (isNaN(newScale)){
            alert("Not a number");
            return;
        }
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
                if (data.info == false){
                    console.log("Scaling circles was not successful, no changes made to file");
                    alert("Change not successful");
                }
                else{
                    alert("Change successful");
                }
            },
            fail: function(error) {
                alert(error);
            }
        });
    };

    document.getElementById('viewAttr').onclick = function () {

        // 1. if no otherAttributes, alert
        let value = "#" + jQuery("#components").children("option:selected").val() + "OA";
        if (jQuery(value).text() <= 0){
            jQuery("#showAttributes").html("No Attributes"); // reset
            alert("There are no attributes to show");
        }
        else{
            // 2. get the variables, such as file name, component type, and component number (aka index in the file)
            let selectedVal = jQuery("#svg").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            let text = jQuery("#components").children("option:selected").text();
            let component = text.match(/[a-zA-Z]+/g).toString(); // regex for getting component
            let componentNumber = text.match(/\d+/g); // regex for getting number

            // 3. get request
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/viewAttrs',
                data: {
                  info: fileName,
                  component: component,
                  index: (componentNumber - 1)
                },
                success: function (data) {
                    if (data.info == null){
                        console.log("Error getting the other attributes");
                        alert("Reuqest failed");
                    }
                    else{
                        // 4. display the string in the div element
                        let otherAttributes = "";
                        for (let i = 0; i < data.info.length; ++i){
                            otherAttributes = otherAttributes + "name: " + data.info[i].name + ", value: " + data.info[i].value + "<br>";
                        }
                        jQuery("#showAttributes").html(otherAttributes);
                    }
                },
                fail: function(error) {
                    alert(error);
                }
            });
        }
    };

    document.getElementById('addAttr').onclick = function () {
        // 1. get the variables, such as file name, component type, and component number (aka index in the file), and new attribute value
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        let text = jQuery("#components").children("option:selected").text();
        let component = text.match(/[a-zA-Z]+/g).toString(); // regex for getting component
        let componentNumber = text.match(/\d+/g); // regex for getting number
        let newName = document.getElementById('nameAttr').value;
        let newValue = document.getElementById('valueAttr').value;
        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/addEditAttrs',
            data: {
              info: fileName,
              component: component,
              index: (componentNumber - 1),
              name: newName,
              value: newValue
            },
            success: function (data) {
                if (data.info == false){
                    console.log("Attribute was not successful, no changes made to file");
                    alert("Change not successful");
                }
                else{
                    alert("Change successful");
                }
            },
            fail: function(error) {
                alert(error);
            }
        });
    };

    document.getElementById('createNewSVG').onclick = function () {
        let fileName = document.getElementById('fileName').value + ".svg";
        if (hasWhiteSpace(fileName)){
            alert("Invalid file name, cannot have whitespace");
            return;
        }

        let svg = {};
        svg.title = document.getElementById('titleName').value;
        svg.descr = document.getElementById('descName').value;

        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/newSVGFile',
            data: {
              info: fileName,
              svg: svg
            },
            success: function (data) {
                if (data.success == false){
                    console.log("SVG File creation was not successful due to invalid file, no changes made to uploads directory");
                    alert("Change not successful");
                }
                else{
                    alert("Change successful");
                }
            },
            fail: function(error) {
                alert(error);
            }
        });
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

            if (data.info == null){
                alert("Error getting files, file contents not generated");
                console.log("Error getting files, file contents not generated");
            }
            else{
                // 2. place a table
                // a. image, title description
                let imageString = "<img src=\"" + fileName + "\" class=\"svgImage\"/>";
                jQuery("#svgViewImg").html(imageString);
                jQuery("#titleText").html(data.info.title);
                jQuery("#descText").html(data.info.description);
                let table = "";

                // 3. for the functionality part of other attributes
                let selectionOA = "<label for=\"componentLabel\">Choose a component: </label><select id=\"components\"></select>";
                jQuery("#viewOtherAttr").html(selectionOA);

                // b. components to access
                let rectIndex = 1;
                for (let i of data.info.rectangles){
                    let data = "Upper left corner: x = " + i.x + i.units + ", y = " + i.y + i.units + ", width = " + i.w + i.units + ", height = " + i.h + i.units;
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Rectangle " + rectIndex + "</td><td>" + data + "</td><td id=\"rect" + rectIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    // only add those that have components
                    let newOption = "<option value=\"rect" + rectIndex + "\">Rectangle " + rectIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++rectIndex;
                }
                let circIndex = 1;
                for (let i of data.info.circles){
                    let data = "Centre: cx = " + i.cx + i.units + ", cy = " + i.cy + i.units + ", r = " + i.r + i.units;
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Circle " + circIndex + "</td><td>" + data + "</td><td id=\"circ" + circIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    let newOption = "<option value=\"circ" + circIndex + "\">Circle " + circIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++circIndex;
                }
                let pathIndex = 1;
                for (let i of data.info.paths){
                    let data = "d = " + i.d;
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Path " + pathIndex + "</td><td>" + data + "</td><td id=\"path" + pathIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    let newOption = "<option value=\"path" + pathIndex + "\">Path " + pathIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++pathIndex;
                }
                let groupIndex = 1;
                for (let i of data.info.groups){
                    let data = i.children + " child elements";
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Group " + groupIndex + "</td><td>" + data + "</td><td id=\"group" + groupIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    let newOption = "<option value=\"group" + groupIndex + "\">Group " + groupIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++groupIndex;
                }

                jQuery("#dynamicBodySVG").html(table);
            }
        },
        fail: function(error) {
            alert(error);
        }
    });

}

function hasWhiteSpace(s) {
  return (/\s/).test(s);
}
