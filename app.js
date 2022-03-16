'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************/

let sharedLib = ffi.Library('./libsvgparser', {
  'validFile' : [ 'bool', [ 'string' ] ],
  'getNumber' : [ 'string', [ 'string'] ],
  'getTitle' : [ 'string', [ 'string'] ],
  'getDescr' : [ 'string', [ 'string'] ],
  'getRectsJSON' : [ 'string', [ 'string'] ],
  'getCircsJSON' : [ 'string', [ 'string'] ],
  'getPathsJSON' : [ 'string', [ 'string'] ],
  'getGroupsJSON' : [ 'string', [ 'string'] ],
  'getAttributesJSON' : [ 'string', [ 'string', 'string', 'int'] ],
  'changeTitle' : [ 'bool', [ 'string', 'string' ] ],
  'changeDescr' : [ 'bool', [ 'string', 'string' ] ],
  'scaleRectangles' : [ 'bool', [ 'string', 'float' ] ],
  'scaleCircles' : [ 'bool', [ 'string', 'float' ] ],
  'setNewAttributes' : [ 'bool', [ 'string', 'string', 'int', 'string', 'string' ] ],
  'createFile' : [ 'bool', [ 'string', 'string' ] ],
  'addCircle' : [ 'bool', [ 'string', 'string' ] ],
  'addRectangle' : [ 'bool', [ 'string', 'string' ] ]
});

app.get('/fileNum', function(req , res){ // get all the file information

  let files = [];
  let data = [];
  let i = 0;

  // 1. get all valid files, place in an array
  fs.readdirSync('./uploads').forEach(file => {
      let extension = file.split('.').pop();
      let filename = "uploads/" + file;
      if ((extension == 'svg') && (sharedLib.validFile(filename) == true)){ // if the file is valid
          files[i] = filename;
          ++i;
      }
      else{
          console.log(filename + " not a valid svg file and not added to the file log panel");
      }
  });

  // 2. get all the information we need
  let size = 0;
  while (size < i){

    // a. get the numbers
    let image = {};

    image.fileName = files[size];
    let currFile = fs.statSync(files[size]);
    image.fileSize = (Math.round((currFile.size) / 1024)); // size in kilobytes
    image.numbers = JSON.parse(sharedLib.getNumber(files[size]));

    // b. lastly, push into the data array
    data.push(image);
    ++size;
  }

  // 2. send the valid files
  res.send( // this will send the error return values
    {
      info: data
    }
  );

});

app.get('/fileInfo', function(req , res){ // get all the file information

  let file = req.query.info;
  let image = {};

  // 1. get all the information we need
  image.title = sharedLib.getTitle(file);
  image.description = sharedLib.getDescr(file);

  // 2. for each shape, get all info on the shapes
  image.rectangles = JSON.parse(sharedLib.getRectsJSON(file));
  image.circles = JSON.parse(sharedLib.getCircsJSON(file));
  image.paths = JSON.parse(sharedLib.getPathsJSON(file));
  image.groups = JSON.parse(sharedLib.getGroupsJSON(file));

  // 2. send the valid object
  res.send( // this will send the error return values
    {
      info: image
    }
  );

});

app.get('/editTitle', function(req , res){

  // get the new value
  let file = req.query.info;
  let newValue = req.query.newValue;
  // update
  let valid = sharedLib.changeTitle(file, newValue);
  if (valid == false){
    console.log("Change unsucessful, title not changed in " + file);
  }
  // return status
  res.send(
    {
      info: valid
    }
  );

});

app.get('/editDesc', function(req , res){ // same as title

  let file = req.query.info;
  let newValue = req.query.newValue;
  let valid = sharedLib.changeDescr(file, newValue);
  if (valid == false){
    console.log("Change unsucessful, description not changed in " + file);
  }
  res.send(
    {
      info: valid
    }
  );

});

app.get('/scaleRects', function(req , res){ // scaling rectangles by a factor

  // get values
  let file = req.query.info;
  let newScale = req.query.newScale;
  // call wrapper function
  let valid = sharedLib.scaleRectangles(file, newScale);
  if (valid == false){
    console.log("Change unsucessful, scaling rectangles not changed in " + file);
  }
  res.send(
    {
      info: valid
    }
  );

});

app.get('/scaleCircs', function(req , res){ // same as rectangles

  let file = req.query.info;
  let newScale = req.query.newScale;
  let valid = sharedLib.scaleCircles(file, newScale);
  if (valid == false){
    console.log("Change unsucessful, scaling circles not changed in " + file);
  }
  res.send(
    {
      info: valid
    }
  );

});

app.get('/viewAttrs', function(req , res){ // to get the other attributes of a component

  // 1. get info
  let file = req.query.info;
  let component = req.query.component;
  let index = req.query.index;

  // 2. get other attibrutes
  let otherAttributes = JSON.parse(sharedLib.getAttributesJSON(file, component, index));
  res.send(
    {
      info: otherAttributes
    }
  );

});

app.get('/addEditAttrs', function(req , res){

  // 1. get info
  let file = req.query.info;
  let component = req.query.component;
  let index = req.query.index;
  let newName = req.query.name;
  let newValue = req.query.value;

  // 2. get other attibrutes
  let valid = sharedLib.setNewAttributes(file, component, index, newName, newValue); // will validate the new attribute in the c function
  if (valid == false){
    console.log("Change unsucessful, attribute not changed in " + file);
  }
  res.send(
    {
      info: valid // returns the result
    }
  );

});

app.get('/newSVGFile', function(req , res){

  // 1. get info
  let svgFile = req.query.info;
  let svg = JSON.stringify(req.query.svg);
  let valid = true;

  // 2. validate file name
  fs.readdirSync('./uploads').forEach(file => {
      let filename = file;
      if (filename == svgFile){ // if the file already exists
          valid = false;
      }
  });

  if (valid == false){
    console.log("Change unsucessful, file name is duplicate " + svgFile);
  }
  else{
    // 3. create the new file
    svgFile = "uploads/" + svgFile;
    valid = sharedLib.createFile(svgFile, svg); // will validate
  }
  res.send(
    {
      success: valid // returns the result
    }
  );

});

app.get('/newSVGCircle', function(req , res){

  // 1. get info
  let svgFile = req.query.info;
  let circle = req.query.circle;
  let valid = sharedLib.addCircle(svgFile, circle); // will validate
  res.send(
    {
      success: valid // returns the result
    }
  );

});

app.get('/newSVGRectangle', function(req , res){

  // 1. get info
  let svgFile = req.query.info;
  let rectangle = req.query.rectangle;
  let valid = sharedLib.addRectangle(svgFile, rectangle); // will validate
  res.send(
    {
      success: valid // returns the result
    }
  );

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
