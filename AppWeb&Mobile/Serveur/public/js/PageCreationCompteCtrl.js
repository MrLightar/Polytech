Application.controller('PageCreationCompteCtrl', ['$scope', '$http', '$state', 'UserService',
function ($scope, $http, $state, UserService){

    // ajoute l'utilisateur
    $scope.addUser = function () {
        if($scope.userName && $scope.userPassword){
            UserService.addUser($scope.userName.trim(), $scope.userPassword.trim(), function(resp){
                if (resp){
                    $state.go('Accueil');
                }

                else {
                    console.log("Erreur de création de compte");
                }
            });
        };
    };

    // suppr utilisateur
    $scope.removeUser = function (user) {
        UserService.deleteUser(user, function (res) {
            if (res) {
            }
        });
    };

    // maj utilisateur
    $scope.updateUser = function(user){
        UserService.updateUser(user, function(res){
            if (res){
            }
        });
    };



}]);