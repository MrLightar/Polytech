//  For log
var fs = require('fs');
var logFile = fs.createWriteStream('log.txt', { flags: 'w' });

//  Database mongo
var mongoose = require('mongoose');
var Schema = mongoose.Schema;

//  for generate guid
var uuidv4 = require('uuid/v4');

//  create connection
mongoose.connect('mongodb://localhost/todo', function (err){
    if (err) { 
        //throw err;
        logFile.write(err.toString());
        console.log('ERREUR : Problème de Connexion au Serveur MongoDB');
        console.log('Erreur ecrite dans fichier de log : log.txt');
        //process.abort();
        process.exit(-1);
    }
    else {
        console.log('mongo connected for tasks');
    }
});

//  declare schema task
var TaskSchema = Schema({
    _id: String,
    name: String,
    user: String,
    done: Boolean
});

//  init model
var TaskModel = mongoose.model('tasks', TaskSchema);


module.exports = {

    //  Add task
    addTask: function(title, name, cb){
        //  create a task
        var task1 = new TaskModel({
            _id: uuidv4(),
            name: title,
            user: name,
            done: false
        });
        
        task1.save(function(err){
            if (err) { throw err; }
            else {
                console.log('task ajouter');
            }
            cb();
        });

    },

    //  Get task
    getTaskSet: function(name, cb){
        TaskModel.find({user: name}, function (err, taskSet){
            cb(taskSet);
        })
    },

    //  Delete task
    deleteTask: function(id, cb){
        TaskModel.deleteOne({_id: id }, function(err){
            if (err){ throw err;}
            else{
                cb();
            }
        })
    },

    //  Update task
    updateTask: function(id, is_done, cb){
        TaskModel.findOne({_id: id}, function (err, task){
            task.done = is_done;
            task.save();
            cb(task);
        });
    },
}