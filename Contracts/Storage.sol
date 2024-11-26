// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.17;

import "@openzeppelin/contracts-upgradeable/proxy/utils/Initializable.sol";
import "@openzeppelin/contracts-upgradeable/proxy/utils/UUPSUpgradeable.sol";

contract Storage is Initializable, UUPSUpgradeable{

    mapping (address=>string) private CheckSum;

    address private ManagerProxyAddress;

    address private owner;

    function initialize() public initializer{
        owner = msg.sender;
    }

    function _authorizeUpgrade(address newImplementation) internal override IsOwner{

    }

     modifier IsCaller(){
        require(ManagerProxyAddress == msg.sender);
        _;
    }

    function StoreVal(string memory _Hash,address _storer) 
    external IsCaller
    {
        CheckSum[_storer] = _Hash;
    }

    function RetriveVal(address _from_pk) external IsCaller view returns(string memory){
        return CheckSum[_from_pk];
    }


    function SetCaller(address _CallerAddress) public IsOwner{
        ManagerProxyAddress = _CallerAddress;
    }

    modifier IsOwner(){
        require(msg.sender == owner);
        _;
    }

    //Test function
    function getAdmin()external view returns(address){
        return owner;
    }
}
