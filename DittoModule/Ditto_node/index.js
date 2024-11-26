Web3 = require("web3")

function init(){
    const web3 = new Web3("ws://127.0.0.1:9290")
    console.log(web3)
}
