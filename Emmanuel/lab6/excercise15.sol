// SPDX-License-Identifier: Unlicensed
pragma solidity 0.8.28;

contract DemoContract {
    
    address public owner;
    struct Receivers {  
        string name;
        uint256 tokens;
    }

    mapping(address => Receivers) public users;

    // Event to log Ether transfers and token purchases
    event TokensPurchased(address buyer, uint amount, uint tokenCount);
    
    modifier onlyOwner() {
        require(msg.sender == owner, "Caller is not the owner");
        _;
    }
    
    constructor() {
        owner = msg.sender;
        users[owner].tokens = 100;
    }
    
    function double(uint _value) public pure returns (uint){
        return _value * 2;
    }
       
    function register(string memory _name) public {
        users[msg.sender].name = _name;
    }
       
    function giveToken(address _receiver, uint256 _amount) public onlyOwner {
        require(users[owner].tokens >= _amount, "Not enough tokens in owner's balance");
        users[owner].tokens -= _amount;
        users[_receiver].tokens += _amount;
    }

    // Function to purchase tokens
    function purchaseTokens() public payable {
        uint tokenPrice = 2 ether; // Price per token
        require(msg.value % tokenPrice == 0, "Send a multiple of token price (2 Ether)");
        uint tokensToBuy = msg.value / tokenPrice;
        require(users[owner].tokens >= tokensToBuy, "Not enough tokens available to purchase");

        users[owner].tokens -= tokensToBuy;
        users[msg.sender].tokens += tokensToBuy;
        emit TokensPurchased(msg.sender, msg.value, tokensToBuy);
    }

    // Function to display the contract's Ether balance
    function getBalance() public view returns (uint) {
        return address(this).balance;
    }
}

