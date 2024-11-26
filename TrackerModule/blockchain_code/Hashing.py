import hashlib
import json

HashList = []


def HashContent(jsonlist):
    #dump = json.dumps(jsonlist, sort_keys=True, indent=2)
    message = bytes(jsonlist,'utf-8')
    hash = hashlib.sha256(message)
    #jsonhash = hashlib.sha256(dump.encode("utf-8")).hexdigest()
    return hash.hexdigest()


def hashToLoad(json_file):
    hash_result = HashContent(json_file)
    HashList.append(hash_result)
    return HashList.pop(0)
