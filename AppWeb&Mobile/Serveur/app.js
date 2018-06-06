var path = require('path');
var express = require('express');

//  parse the param
var bodyParser = require('body-parser');

//  include repository for mango
var dataTaskLayer = require('./repository/dataTaskLayer.js');
var dataUserLayer = require('./repository/dataUserLayer.js');


//  Variable
var app = express();
var port = 8090;

//  Add headers
app.use(function(req, res, next){
    //  Website you wish to allow
    res.setHeader('Access-Control-Allow-Origin', '*');
    
    //  Request methods you wish to allow
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');
    
    //  Request headers you wish to allow
    res.setHeader('Access-Control-Allow-Headers', 'X-requested-With, content-type');
    
    //  set to true if ....  include cookies in the request ...
    res.setHeader('Access-Control-Allow-Credentials', true);
    
    //  pass to the next layer of middleware
    next()
})

//  init parser with express
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true}));

//  authorize access to public dir
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', function(req, res) {
    //console.log(req);
    res.setHeader('Content-Type', 'text/plain');
});


//  TODO LIST TASK POST
app.post('/addTask', function(req, res){
    console.log("TASK ADD");
    dataTaskLayer.addTask(req.body.title, req.body.name, function(){
        var obj = {
            succes: true
        }

        res.send(obj);
    });
})




app.post('/getTaskSet', function(req, res){
    dataTaskLayer.getTaskSet(req.body.name, function(taskSet){
        var obj = {
            success: true,
            taskSet: taskSet
        };

        for (var i = 0; i < taskSet.length; i++){
            console.log('Get task set')
        }

        res.send(obj);
    })
})

app.post('/deleteTask', function (req, res) {
    dataTaskLayer.deleteTask(req.body._id, function (result) {
        var obj = {
            success: true,
        };

        res.send(obj);
    });
});

app.post('/updateTask', function(req, res){
    dataTaskLayer.updateTask(req.body._id, req.body.done, function(result){
        var obj = {
            succes: true,
        };

        res.send(obj);
    });
});


//  TODO LIST USER POST
app.post('/addUser', function(req, res){
    console.log("USER ADD");
    dataUserLayer.addUser(req.body.name, req.body.password, function(){
        var obj = {
            succes: true
        }

        res.send(obj);
    });
})

app.post('/getUser', function(req, res){
    console.log("USER GET");
    dataUserLayer.getUser(req.body.name, req.body.password, function(user){
        if (user == null){
            var obj = {succes: false}
        }
        else {
            var obj = {
                succes: true,
                user: user
            }
        }
        res.send(obj);
    });
})



console.log('Serveur démarré port :' + port);
app.listen(port);
