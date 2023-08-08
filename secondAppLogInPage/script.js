var myApp = angular.module("myLogInApp", []);
myApp.controller("myLogInAppController", function($scope){
	
	$scope.register = function (){
			$scope.msg = "Welcome "+$scope.user.firstname+ " ! You have signed in ";
	}
	
})