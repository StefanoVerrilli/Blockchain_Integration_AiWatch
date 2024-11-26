from web3 import Web3
from web3.middleware import geth_poa_middleware

class Connection(object):
    __instance = None

    def __new__(self,endpoint):
        if not self.__instance:
            self.__instance = super(Connection, self).__new__(self)
            self.w3 = Web3(Web3.HTTPProvider(endpoint))
            self.w3.middleware_onion.inject(geth_poa_middleware, layer=0)
        return self.__instance