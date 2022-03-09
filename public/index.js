// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/fileInfo',   //The server endpoint we are connecting to
        data: {
          info: ""
        },
        success: function (data) {
            jQuery('#blah').html("On page load, received string " + data.info + " from server");
            console.log(data.info);
        },
        fail: function(error) {
            $('#blah').html("On page load, received error from server");
            console.log(error);
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    // START OF MY CODE Default: hidden objects
    document.getElementById("addAttrform").style.display="none";
    document.getElementById("showOtherAttr").style.display="none";

    document.getElementById('titleform').onclick = function () {
        let role = $(this).find('.editT');
        let value = $(this).siblings(); // search DOM
        let values = value.text();

        let newTitle = value.find('.titleTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            $(this).siblings().html('<input type="textbox" class="titleTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newTitle != "") {
                $(this).siblings().html('<span id="titleText">' + newTitle + '</span>');
                role.html("Edit");
            }
            else {
                alert("Please Fill the field");
            }
        }
    };

    document.getElementById('descform').onclick = function () {
        let role = $(this).find('.editD');
        let value = $(this).siblings(); // search DOM
        let values = value.text();

        let newDesc = value.find('.descTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            $(this).siblings().html('<input type="textbox" class="descTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newDesc != "") {
                $(this).siblings().html('<span id="descText">' + newDesc + '</span>');
                role.html("Edit");
            }
            else {
                alert("Please Fill the field");
            }
        }
    };

    document.getElementById('viewAttr').onclick = function () {
        document.getElementById("showOtherAttr").style.display="block";
    };

    document.getElementById('attrform').onclick = function () {
        let role = $(this).find('.editA');
        let value = $(this).siblings(); // search DOM
        let values = value.text();

        let newAttr = value.find('.attrTextBox').val();

        if (role.html() == "Edit") {
            role.html("Save");
            $(this).siblings().html('<input type="textbox" class="attrTextBox" value="" placeholder="' + values + '">');
            values = "";
        }
        else {
            if (newAttr != "") {
                $(this).siblings().html('<span id="attrText">' + newAttr + '</span>');
                role.html("Edit");
            }
            else {
                alert("Please Fill the field");
            }
        }
    };

    document.getElementById('aAttr').onclick = function () {
        document.getElementById("addAttrform").style.display="block";
        let newName = document.getElementById('nameAttr').value;
        let newValue = document.getElementById('valueAttr').value;
        document.getElementById('attrSubmit').onclick = function (){
            console.log("got this element: " + newName + " " + newValue);
        };
    };

    // END OF MY CODE

});
