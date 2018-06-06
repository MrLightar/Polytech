Application.controller('PageTodoCtrl', ['$scope', '$http', 'TodoService', 'UserService',
function ($scope, $http, TodoService, UserService){

    // liste des taches
    $scope.taskSet = [];

    // compte le nb de taches faite
    $scope.howManyDone = function(){
        count = 0;
        $scope.taskSet.forEach(element => {
            if(element.done){
                count++
            }
        });
        return count;
    };

    // compte le nb de taches non faite
    $scope.howManyNotDone = function(){
        return $scope.taskSet.length
            -$scope.howManyDone();
    };

    // ajoute une tache
    $scope.addT = function () {
        if($scope.taskName){
            console.log(UserService.userName())
            TodoService.addTask($scope.taskName.trim(), UserService.userName(), function(resp){
                if (resp){
                    $scope.refreshT();
                };
            });
        };
    };

    // maj des taches de la todolist
    $scope.refreshT = function(){
        TodoService.getTaskSet(UserService.userName(), function (taskSet){
           $scope.taskSet = taskSet;
           console.log(taskSet);
        });
    };

    //suppresion d'une tache
    $scope.removeTask = function (task) {
        TodoService.deleteTask(task, function (res) {
            if (res) {
                $scope.refreshT();
            }
        });
    };

    $scope.updateTask = function(task){
        TodoService.updateTask(task, function(res){
            if (res){
                $scope.refreshT();
            }
        });
    };

    $scope.refreshT();


}]);