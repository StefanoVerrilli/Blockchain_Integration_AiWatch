from os.path import join, dirname
import json

class Contract(object):
    __instance = None

    def __new__(self, w3, address):
        print("contract instance")
        if not self.__instance:
            self.__instance = super(Contract, self).__new__(self)
            with open(join(dirname(__file__),'abi.json')) as f:
                info_json = json.load(f)
            self.contract = w3.eth.contract(address=address, abi=info_json)
        return self.__instance