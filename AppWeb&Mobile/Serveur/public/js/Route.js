Application.config(function($stateProvider){

    // ecran d'accueil et de connexion
    var homeState = {
        name: 'Accueil',
        url: 'Accueil',
        templateUrl: 'page/Connexion.html',
        controller: 'PageConnexionCtrl'
    };

    // ecran de creation
    var pageCreationState = {
        name: 'PageCreationCompte',
        url: 'CreationCompte',
        templateUrl: 'page/CreationCompte.html',
        controller: 'PageCreationCompteCtrl'
    }

   // ecran de la todolist
    var pageTodoState = {
        name: 'PageTodo',
        url: 'Todo',
        templateUrl: 'page/pageTodo.html',
        controller: 'PageTodoCtrl'
    };

    

    $stateProvider.state(homeState);
    $stateProvider.state(pageCreationState);
    $stateProvider.state(pageTodoState);


});
