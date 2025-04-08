// SPDX-License-Identifier: MIT

// Exercise 1
pragma solidity >=0.5.0 <0.8.29;

// Exercise 2 & 3
contract Factory {
    // Exercise 9
    event NewProduct(uint ArrayProductId, string name, uint id);

    uint idDigits = 16;
    uint idModulus = 10 ** idDigits;

    // Exercise 4
    struct Product {
        string Name; // Product name
        uint Id;
    }

    // Exercise 5
    Product[] public products;

    // Exercise 10
    mapping(uint => address) public productToOwner;
    mapping(address => uint) ownerProductCount;

    // Exercise 6 & 9
    function _createProducts(string memory _name, uint _id) private {
        Product memory newProduct = Product(_name, _id);
        products.push(newProduct);
        uint productId = products.length - 1;
        emit NewProduct(productId, _name, _id);
    }

    // Exercise 7 & 8
    function _generateRandomId(string memory _str) private view returns (uint) {
        uint rand = uint(keccak256(abi.encodePacked(_str)));
        return rand % idModulus;
    }
    function createRandomProduct(string memory _name) public {
        uint randId = _generateRandomId(_name);  // Generate a random ID based on the name
        _createProducts(_name, randId);          // Create a new product with the random ID
    }

    // Exercise 11
    function assignOwnership(uint _productId) public {
        productToOwner[_productId] = msg.sender;
        ownerProductCount[msg.sender]++;
    }
    //excercise12
    function getProductsByOwner(address _owner) external view returns (uint[] memory) {
        uint[] memory result = new uint[](ownerProductCount[_owner]);
        uint counter = 0;

        for (uint i = 0; i < products.length; i++) {
            if (productToOwner[i] == _owner) {
                result[counter] = i;
                counter++;
            }
        }
        return result;
    }
}