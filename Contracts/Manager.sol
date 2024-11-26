// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.8.7;

import "@openzeppelin/contracts-upgradeable/proxy/utils/Initializable.sol";
import "@openzeppelin/contracts-upgradeable/proxy/utils/UUPSUpgradeable.sol";

interface Admin{
    function getAdmins() external view returns (address[] memory);
}

interface StorageContract{
    function StoreVal(string memory _Hash,address from) external;
    function RetriveVal(address _from_pk) external view returns(string memory);
}

contract Manager is Initializable, UUPSUpgradeable{

    struct Receiver {
        address to;
        bool exists;
    }

    mapping (address=>Receiver) private correspondings;

    address[] adminList;

    prova AdminContractInstance;

    StorageContract StorageContractProxyInstance;

    function Inizialize(address _AdminAddress,address _StorageProxyAddress) external{
        address myaddress = 0xb848ef765E289762e9BE66a38006DDc4D23AeF24; 
        AdminContractInstance = prova(myaddress);
        StorageContractProxyInstance = StorageContract(_StorageProxyAddress);
        //adminList = AdminContractInstance.getAdmins();
    }
    
    function _authorizeUpgrade(address newImplementation) internal override OnlyOwnerof{

    }

// Interface for interaction with Storage smart contract

    function StoreInterface(string memory Hash) external {
        if(!MappingExists(msg.sender))
            revert();
        StorageContractProxyInstance.StoreVal(Hash, msg.sender);
    }


    function RetriveInterface(address _receiver) external view{
        if(!(correspondings[msg.sender].to == _receiver))
            revert();
        StorageContractProxyInstance.RetriveVal(msg.sender);
    }
// END --Interface for storage smart contract

// Starting corresponding managment for ensure the correct usage of the pipeline

    function MappingExists(address _key) private view returns(bool exists) {
        return correspondings[_key].exists;
    }

    function newCorresponding(address _sender,address _receiver) 
    external OnlyOwnerof 
    validAddress(_sender) 
    validAddress(_receiver)
    returns(bool isAdded)
    {
        require(!MappingExists(_sender));
        correspondings[_sender].to = _receiver;
        correspondings[_sender].exists = true;
        return true;
    }

    function deleteCorresponding(address _sender)
    external OnlyOwnerof
    validAddress(_sender)
    returns(bool isDeleted) {
        require(MappingExists(_sender));
        correspondings[_sender].exists = false;
        return true;
    }

    function updateEntity(address _sender, address _receiver) 
    external OnlyOwnerof
    validAddress(_sender)
    validAddress(_receiver)
    returns(bool isUpdated) {
        require(MappingExists(_sender));
        correspondings[_sender].to = _receiver;
        return true;
    }

    // -- Correspondings managment 

    // Owner checkings to ensure the caller is authorized

    modifier OnlyOwnerof(){
        //adminList = AdminContractInstance.getAdmins();
        //for(uint i=0;i<adminList.length;i++){
        //    if(adminList[i] == msg.sender)
                _;
        //}
        //revert("Not Allowed");
    }
    // -- Owner checkings

    modifier validAddress(address _address){
        require(_address != address(0));
        _;
    }


    //Test function
    function getAdminList() external view returns(address [] memory){
        return AdminContractInstance.getAdmins();
    }

}