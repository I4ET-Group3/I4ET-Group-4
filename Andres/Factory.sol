// SPDX-License-Identifier: MIT
pragma solidity >=0.5.0 <0.9.0;

contract Factory {
    struct Product {
        string name; // Product name
        uint id;     // Product ID
    }

    Product[] public products; // Public array to store all smart products

    uint public idDigits = 16; // A 16-digit identifier for products
    uint public idModulus = 10 ** idDigits; // Modulus for calculating based on idDigits

    // Event to notify the front-end when a new product is created
    event NewProduct(uint ArrayProductId, string name, uint id);

    // Mappings to store product ownership and user age
    mapping(uint => address) public productToOwner; // Maps product ID to owner's address
    mapping(address => uint) public ownerProductCount; // Maps owner address to their product count
    mapping(uint => uint) public age; // Maps user ID to their age

    // Modifier to check if a user is older than a specified age
    modifier olderThan(uint _age, uint _userId) {
        require(age[_userId] >= _age, "User does not meet the required age."); // Check user's age
        _;
    }

    // Private function to create a smart product
    function createProduct(string memory _name, uint _id) private {
        products.push(Product(_name, _id)); // Add new product
        uint productId = products.length - 1; // Calculate product index
        productToOwner[productId] = msg.sender; // Assign ownership to the creator
        ownerProductCount[msg.sender]++; // Increment owner's product count
        emit NewProduct(productId, _name, _id); // Emit event with product details
    }

    // Private function to generate a random ID based on a string input
    function _generateRandomId(string memory _str) private view returns (uint) {
        uint rand = uint(keccak256(abi.encodePacked(_str))); // Generate pseudo-random hexadecimal
        return rand % idModulus; // Restrict ID to 16 digits
    }

    // Public function to create a random product
    function createRandomProduct(string memory _name) public {
        uint randId = _generateRandomId(_name); // Generate random ID using _name
        createProduct(_name, randId); // Create a new product with the generated ID
    }

    // Public function to assign ownership of a product
    function assignOwnership(uint productId) public {
        require(productId < products.length, "Product ID is invalid."); // Check that the product exists
        productToOwner[productId] = msg.sender; // Assign ownership to the caller
        ownerProductCount[msg.sender]++; // Increment owner's product count
    }

    // External view function to get products by owner
    function getProductsByOwner(address _owner) external view returns (uint[] memory) {
        uint counter = 0; // Counter for matching product indices
        uint[] memory result = new uint[](ownerProductCount[_owner]); // Initialize the result array

        for (uint i = 0; i < products.length; i++) {
            if (productToOwner[i] == _owner) { // Check ownership
                result[counter] = i; // Add product index to result array
                counter++; // Increment counter
            }
        }

        return result; // Return array of product indices owned by _owner
    }

    // Function to validate users based on their age
    function validationUsers(uint _userId) public view olderThan(18, _userId) {
        // Body intentionally left blank as requested
    }
}