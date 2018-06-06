Application.factory('TodoService', ['$http',
function($http) {

    var serv = {};

    // ajoute une tache au serveur
    serv.addTask = function(task, name, cb){
        $http.post('http://localhost:8090/addTask', {title: task, name: name}).then(function (resp){
            cb(resp.data.succes);
        }, function(res){
            console.log("ERROR = " + res.data.errorSet);
        });
    };

    // récupère les taches du serveur
    serv.getTaskSet = function(name, cb){
        $http.post('http://localhost:8090/getTaskSet', {name: name}).then(function(resp){
            cb(resp.data.taskSet);
        }, function(res){
            console.log("ERROR = " + res.data.errorSet);
        })
    };

    // supprime une tache du serveur
    serv.deleteTask = function (task, cb) {
        $http.post('http://localhost:8090/deleteTask', task).then(function (resp) {
            cb(resp.data.success);
        }, function (res) {
            console.log("ERROR = " + res);
        });
    };

    // maj serveur
    serv.updateTask = function(task, cb) {
        var req = {
            id:task._id,
            name:task.name,
            done:task.done
        };
        $http.post('http://localhost:8090/updateTask', task).then(function(resp){
            cb(resp.data.succes);

        }, function(res){
            console.log("ERROR = " + res);
        });
    };

    return serv;
}]);
