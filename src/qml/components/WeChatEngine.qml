pragma Singleton

import QtQuick

Item {
    enum BackupType {
        BackupType_IOS = 0,
        BackupType_WIN,
        BackupType_UNKNOWN
    }

    property string defaultHeadImg: "qrc:/assets/images/DefaultProfileHead@2x.png"
    property string notExistImageOrVideo: "qrc:/assets/icons/image-not-found.png"

    property int openFolderBackupType: -1
    property int currentFolderBackupType: -1
    property var currentLoginUser
    property string currentLoginUserName

    function detectSecretKey() {
        return pWeChat.detectLoginUserSecretKey()
    }

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
    function listLoginUserNames() {
        return pWeChat.listLoginUserNames()
    }

    function loadLoginUser(userName, secretKey) {
        currentLoginUserName = userName
        currentLoginUser = pWeChat.loadLoginUser(userName, secretKey)
    }

    function isCurrentLoginUser(user) {
        return user["userID"] === currentLoginUser["userID"]
    }

    function getCurrentLoginUserHeadImgUrl() {
        return getUserHeadImgUrl(currentLoginUser)
    }

    function getUserHeadImgUrl(user) {
        // http url
        if ("headImg" in user && user["headImg"] != "") {
            return user["headImg"]
        }
        return defaultHeadImg
    }

    function listFriends(start, count) {
        return pWeChat.listFriends(start, count, false)
    }

    // page 从1开始
    function listMessages(friendID, page, count) {
        return pWeChat.listMessages(friendID, page, count)
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

    function playAudio(friendID, message) {
        pWeChat.playAudio(friendID, message)
    }

    function getMsgImageUrl(metadata) {
        if (openFolderBackupType == WeChatEngine.BackupType_WIN) {
            if (metadata["thumb"] !== "") {
                return "image://wechatimg/" + metadata["thumb"]
            }
            else if (metadata["src"] !== "") {
                return "image://wechatimg/" + metadata["src"]
            }
        }
        else {
            if (metadata["thumb"] !== "") {
                return "file:///" + metadata["thumb"]
            }
            else if (metadata["src"] !== "") {
                return "file:///" + metadata["src"]
            }
        }
        return notExistImageOrVideo
    }

    function getMsgVideoThumbImageUrl(metadata) {
        if ("thumb" in metadata && metadata["thumb"] != "") {
            return "file:///" + metadata["thumb"]
        }
        return notExistImageOrVideo
    }

    function getMsgVideoUrl(metadata) {
        if ("raw" in metadata && metadata["raw"] != "") {
            return "file:///" + metadata["src"]
        }
        else if ("src" in metadata && metadata["src"] != "") {
            return "file:///" + metadata["src"]
        }
        return ""
    }
}