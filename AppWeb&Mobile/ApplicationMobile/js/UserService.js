Application.factory('UserService', ['$http',
function($http) {

    var serv = {};

    //  Variable utilisateur
    var userName;

    // ajoute un utilisateur au serveur
    serv.addUser = function(name, password, cb){
        $http.post('http://localhost:8090/addUser', {name: name, password: password}).then(function (resp){
            cb(resp.data.succes);
        }, function(res){
            console.log("ERROR = " + res.data.errorSet);
        });
    };

    //récupère un utilisateur du serveur
    serv.getUser = function(name, password, cb){
        $http.post('http://localhost:8090/getUser', {name: name, password: password}).then(function(resp){
            if (resp.data.succes == true){
                userName = resp.data.user.userID;
                cb(resp.data.user);
            }
            else {
                cb(null);
            }
        }, function(res){
            console.log("ERROR = " + res.data.errorSet);
        })
    };

    // régupère le login
    serv.userName = function(){
        return userName;
    }


    return serv;
}]);
