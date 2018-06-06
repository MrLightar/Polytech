//  For log
var fs = require('fs');
var logFile = fs.createWriteStream('log.txt', { flags: 'w' });

//  Database mongo
var mongoose = require('mongoose');
var Schema = mongoose.Schema;

//  for generate guid
var uuidv4 = require('uuid/v4');

//  create connection
mongoose.connect('mongodb://localhost/users', function (err){
    if (err) { 
        //throw err;
        logFile.write(err.toString());
        console.log('ERREUR : Problème de Connexion au Serveur MongoDB');
        console.log('Erreur ecrite dans fichier de log : log.txt');
        //process.abort();
        process.exit(-1);
    }
    else {
        console.log('mongo connected for users');
    }
});

//  declare schema user
var UserSchema = Schema({
    _id: String,
    userID: String,
    userPassword: String
});

//  init model
var UserModel = mongoose.model('users', UserSchema);


module.exports = {

    //  Add user
    addUser: function(name, password, cb){
        //  create user
        var newUser = new UserModel({
            _id: uuidv4(),
            userID: name,
            userPassword: password
        });
        
        console.log(name + " " + password);

        newUser.save(function(err){
            if (err) { throw err; }
            else {
                console.log('user ajouter');
            }
            cb();
        });

    },

    //  Get user
    getUser: function(name, password, cb){
        UserModel.findOne({userID: name, userPassword: password}, function (err, user){
            if (err) { throw err; }
            else {
                cb(user);
            }
        })
    },


}