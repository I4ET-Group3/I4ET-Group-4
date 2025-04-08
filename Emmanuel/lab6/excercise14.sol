// SPDX-License-Identifier: MIT
pragma solidity ^0.8.28;

contract UserAge {
    // Mapping from user ID to age
    mapping(uint => uint) public age;

    // Modifier to check if the user's age is greater than or equal to a certain value
    modifier olderThan(uint _age, uint _userId) {
        require(age[_userId] >= _age, "User is not old enough.");
        _; // Continue execution of the function
    }

    // Function to validate if the user is an adult
    function validateUser(uint _userId) public view olderThan(18, _userId) {
        // Function body can be implemented as needed
    }
}
