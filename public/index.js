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

    // make certain compoenents not visible
    document.getElementById("addCircleForm").style.display="none";
    document.getElementById("addRectangleForm").style.display="none";
    document.getElementById("addAttrForm").style.display="none";
    document.getElementById("editShapeAttrSection").style.display="none";

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

    document.getElementById('addShapeCirc').onclick = function () {
        document.getElementById("addCircleForm").style.display="block";
        document.getElementById('addCirc').onclick = function () {
            let x = Number.parseFloat(document.getElementById('CX').value);
            let y = Number.parseFloat(document.getElementById('CY').value);
            let r = Number.parseFloat(document.getElementById('CR').value);
            if (isNaN(x) || isNaN(y) || isNaN(r)){
                alert("Coordinates and Radius must be a number");
                return;
            }
            let units = document.getElementById('CU').value;
            if (units == "Units"){
                alert("Please enter a valid unit, or leave empty");
                return;
            }
            else if (units.length > 50){
                alert("Unit must be less than 50 characters");
                return;
            }
            let circle = JSON.stringify({
                "cx": x,
                "cy": y,
                "r": r,
                "units": units
            });
            let selectedVal = jQuery("#svgShape").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/newSVGCircle',
                data: {
                  info: fileName,
                  circle: circle
                },
                success: function (data) {
                    if (data.success == false){
                        console.log("Circle creation was not successful due to invalid circle");
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

    document.getElementById('addShapeRect').onclick = function () {
        document.getElementById("addRectangleForm").style.display="block";
        document.getElementById('addRect').onclick = function () {
            let x = Number.parseFloat(document.getElementById('RX').value);
            let y = Number.parseFloat(document.getElementById('RY').value);
            let width = Number.parseFloat(document.getElementById('RW').value);
            let height = Number.parseFloat(document.getElementById('RH').value);
            if (isNaN(x) || isNaN(y) || isNaN(width) || isNaN(height)){
                alert("Coordinates and Dimensions must be a number");
                return;
            }
            let units = document.getElementById('RU').value;
            if (units == "Units"){
                alert("Please enter a valid unit, or leave empty");
                return;
            }
            else if (units.length > 50){
                alert("Unit must be less than 50 characters");
                return;
            }
            let rectangle = JSON.stringify({
                "x": x,
                "y": y,
                "width": width,
                "height": height,
                "units": units
            });
            let selectedVal = jQuery("#svgShape").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/newSVGRectangle',
                data: {
                  info: fileName,
                  rectangle: rectangle
                },
                success: function (data) {
                    if (data.success == false){
                        console.log("Rectangle creation was not successful due to invalid circle");
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

    document.getElementById('addShapeAttr').onclick = function () {
        document.getElementById("addAttrForm").style.display="block";
        document.getElementById('addAttr').onclick = function () {
            // 1. get the variables, such as file name, component type, and component number (aka index in the file), and new attribute value
            let selectedVal = jQuery("#svg").children("option:selected").val();
            let fileName = "uploads/" + selectedVal;
            let text = jQuery("#components").children("option:selected").text();
            let component = text.match(/[a-zA-Z]+/g).toString(); // regex for getting component
            let componentNumber = text.match(/\d+/g); // regex for getting number
            let newName = document.getElementById('nameAttr').value;
            let newValue = document.getElementById('valueAttr').value;
            addEditShapeAttr(fileName, component, componentNumber, newName, newValue);
        };
    };

    document.getElementById('editShapeAttr').onclick = function () {

        // to make more user friendly, and less error prone, will fill in the required values for them
        document.getElementById("editShapeAttrSection").style.display="block";

        // 1. get the variables, such as file name, component type, and component number (aka index in the file), and new attribute value
        let selectedVal = jQuery("#svg").children("option:selected").val();
        let fileName = "uploads/" + selectedVal;
        let text = jQuery("#components").children("option:selected").text();
        let component = text.match(/[a-zA-Z]+/g).toString(); // regex for getting component
        let componentNumber = text.match(/\d+/g); // regex for getting number

        let value = "#" + jQuery("#components").children("option:selected").val() + "OA";

        // 2. create a list of all the existing attributes that can be editable
        let selection = "<label for=\"attrLabel\">Choose an attribute: </label><select id=\"editAttrList\"></select>";
        jQuery("#editShapeAttrSection").html(selection);

        let editable = [];
        if (component == "Rectangle"){
            editable.push("x");
            editable.push("y");
            editable.push("width");
            editable.push("height");
        }
        else if (component == "Circle"){
            editable.push("cx");
            editable.push("cy");
            editable.push("r");
        }
        else if (component == "Path"){
            editable.push("d");
        }

        // 3. add the list to the drop down menu
        for (let i = 0; i < editable.length; ++i) {
            let newOption = "<option value=\"" + editable[i] + "\">" + editable[i] + "</option>";
            jQuery("#editAttrList").append(newOption);
        }

        if (jQuery(value).text() > 0){
            // 4. get request for other attributes - since idk how to return values for asynchronous functions :(
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
                    if (data.info != null){
                        // add to list of existing, editable components
                        for (let i = 0; i < data.info.length; ++i){
                            let newOption = "<option value=\"" + data.info[i].name + "\">" + data.info[i].name + "</option>";
                            jQuery("#editAttrList").append(newOption);
                        }
                    }
                },
                fail: function(error) {
                    alert(error);
                }
            });
        }

        // 5. form to actually submit changes
        let submit = "<input type=\"text\" class=\"form-control\" id=\"newEditAttr\" value=\"New Value\" placeholder=\"Placeholder\"><br><button id=\"submitChanges\" class=\"btn btn-secondary\">Submit</button>";
        jQuery("#editShapeAttrSection").append(submit);

        document.getElementById('submitChanges').onclick = function () {
            let name = jQuery("#editAttrList").children("option:selected").val();
            let newValue = document.getElementById('newEditAttr').value;
            addEditShapeAttr(fileName, component, componentNumber, name, newValue);
            reload = location.reload();
       };

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

/*
                let svg = fileName.split('/').pop();
                let newOption = "<option value=\"SVG\">" + svg + "</option>";
                jQuery("#components").append(newOption);
*/
                // b. components to access
                let rectIndex = 1;
                for (let i of data.info.rectangles){
                    let data = "Upper left corner: x = " + i.x + i.units + ", y = " + i.y + i.units + ", width = " + i.w + i.units + ", height = " + i.h + i.units;
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Rectangle " + rectIndex + "</td><td id=\"rect" + rectIndex + "D\">" + data + "</td><td id=\"rect" + rectIndex + "OA\">" + otherAttrNum + "</td></tr>";
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
                    let newRow = "<tr><td>Circle " + circIndex + "</td><td id=\"circ" + circIndex + "D\">" + data + "</td><td id=\"circ" + circIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    let newOption = "<option value=\"circ" + circIndex + "\">Circle " + circIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++circIndex;
                }
                let pathIndex = 1;
                for (let i of data.info.paths){
                    let data = "d = " + i.d;
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Path " + pathIndex + "</td><td id=\"path" + pathIndex + "D\">" + data + "</td><td id=\"path" + pathIndex + "OA\">" + otherAttrNum + "</td></tr>";
                    table = table + newRow;

                    let newOption = "<option value=\"path" + pathIndex + "\">Path " + pathIndex + "</option>";
                    jQuery("#components").append(newOption);
                    ++pathIndex;
                }
                let groupIndex = 1;
                for (let i of data.info.groups){
                    let data = i.children + " child elements";
                    let otherAttrNum = i.numAttr;
                    let newRow = "<tr><td>Group " + groupIndex + "</td><td id=\"group" + groupIndex + "D\">" + data + "</td><td id=\"group" + groupIndex + "OA\">" + otherAttrNum + "</td></tr>";
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

function addEditShapeAttr(fileName, component, componentNumber, newName, newValue){

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
alert("return value: " + data.info);
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

}
