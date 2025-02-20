pragma Singleton

import QtQuick

Item {
    enum BackupType {
        BackupType_IOS = 0,
        BackupType_WIN,
        BackupType_UNKNOWN
    }

    property int openFolderBackupType: -1
    property int currentFolderBackupType: -1
    property var currentLoginUser
    property string currentLoginUserName

    function initContextFromPath(folder) {
        var index = pWeChat.detectBackupType(folder);
        if (index >= 0 && index < 2) {
            if (pWeChat.initContextFromPath(folder)) {
                openFolderBackupType = index
                return 0        // OK
            }
        }
        return 1;               // NOK
    }

    // stringlist
    function listLoginUsers() {
        return pWeChat.listLoginUsers()
    }

    function loadLoginUser(userName, secretKey) {
        currentLoginUserName = userName
        currentLoginUser = pWeChat.loadLoginUser(userName, secretKey)
    }

    function loadFriends() {
        return pWeChat.listFriends()
    }

    function loadMessages() {

    }

    function performAsyncOperation(operation, done) {
        new Promise((resolve, reject) => {
            operation()
            resolve()
        }).then(result => {
            done()
        }).catch(error => {
            console.error(error); // 处理错误情况
        });
    }
}