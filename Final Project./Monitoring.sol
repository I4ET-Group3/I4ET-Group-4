// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

contract Monitoring {

    // Sensor variables
    uint256 public Temperature;
    uint256 public Humidity;
    uint256 public smoke;
    string public flame;
    
    // Engines variables
    uint256 public Left_Prop_RPM;
    uint256 public Right_Prop_RPM;
    uint256 public rpmLimit = 1800;
    uint256 public maxRPMDuration = 60; 

    // System state
    bool public enginesActive = true;
    bool public batteriesActive = true;
    bool public negligenceDetected = false;
    bool public fineIssued = false;

    mapping(address => bool) public insurerAccess;

    // Events
    event Alert(string message);
    event FineIssued(string reason);
    event SystemShutdown(string component);

    function compareStrings(string memory a, string memory b) internal pure returns (bool) {
        return keccak256(abi.encodePacked(a)) == keccak256(abi.encodePacked(b));
    }


    // Update sensors individually from MQTT
    function updateTemperature(uint256 temp) public {
        Temperature = temp;
        evaluateRisk();
    }

    function updateHumidity(uint256 hum) public {
        Humidity = hum;
        evaluateRisk();
    }

    function updateSmoke(uint256 smokeLevel) public {
        smoke = smokeLevel;
        evaluateRisk();
    }

    function updateFlame(string memory fireStatus) public {
        flame = fireStatus;
        evaluateRisk();
    }

    function updateMotorData(uint256 leftRPM, uint256 rightRPM) public {
        Left_Prop_RPM = leftRPM;
        Right_Prop_RPM = rightRPM;
        checkEngineOveruse();
    }

    // Assess whether there is serious risk
    function evaluateRisk() internal {
        if (Temperature > 40 || Humidity > 80 || smoke > 50 || compareStrings(flame, "FIRE")) {
            negligenceDetected = true;
            emit Alert("Risk detected, shutting down systems");
            shutdownEngines();
            shutdownBatteries();
        }
    }

    // Turn off engines if there is danger
    function shutdownEngines() internal {
        enginesActive = false;
        emit SystemShutdown("Engines shut down due to overheating");
    }

    // Turn off batteries if there is an electrical risk
    function shutdownBatteries() internal {
        batteriesActive = false;
        emit SystemShutdown("Batteries deactivated due to fire risk");
    }

    // Evaluate whether the engines operated too long at maximum RPM
    function checkEngineOveruse() internal {
        if (Left_Prop_RPM > rpmLimit && Right_Prop_RPM > rpmLimit) {
            fineIssued = true;
            emit FineIssued("Penalty applied for prolonged use of maximum RPM");
        }
    }

    // Consultation for insurance compay
    function reportToInsurers() public view returns (bool) {
        require(insurerAccess[msg.sender], "Unauthorized");
        return negligenceDetected;
    }

    function grantInsurerAccess(address insurer) public {
        insurerAccess[insurer] = true;
    }
}