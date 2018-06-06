Application.controller('PageConnexionCtrl', ['$scope', '$http', '$state', 'UserService',
function ($scope, $http, $state, UserService){

    // récupère l'utilisateur
    $scope.getUser = function(){
        UserService.getUser($scope.userName, $scope.userPassword, function (user){
           if (user == null){
               $scope.Error = 'ERREUR Nom d\' utilisateur ou mot de pass incorrect'
           }
           else {
               $state.go('PageTodo')
           }
        });
    };

    // redirige sur la page de création
    $scope.goToCreation = function(){
        $state.go('PageCreationCompte');
    };

}]);