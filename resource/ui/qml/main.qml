import QtQuick 2.9
import QtQuick.Window 2.2
//import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import com.may.myplayer 1.0

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: qsTr("XPlayer")
    property bool fullscreen: false


    //背景图片
    Image {
        id: backGround
        width: parent.width
        height: parent.height
        source: xplay.backGround ? "file:///" + xplay.backGround : ""
        //fillMode: Image.TileHorizontally
        smooth: true
    }
    XPlay{
        id:xplay
    }


    Slider {
        id: playPos;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 50
        anchors.rightMargin: 50
        anchors.topMargin: 10
        stepSize: 0.001

    }

    // 打开文件按钮
    Button {
        id: openFileBtn
        width: root.controlAreaButtonWidth
        height: root.controlAreaButtonHeight
        anchors.left: playPos.left
        anchors.leftMargin: 50
        anchors.top: playPos.bottom
        contentItem: Image {
            source: "qrc:///image/openFile.png"
            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectFit
        }
    }


    // 快退按钮
     Button {
         id: goBackBtn
         width: root.controlAreaButtonWidth
         height: root.controlAreaButtonHeight
         anchors.right: isPlayBtn.left
         anchors.top: openFileBtn.top
         contentItem: Image {
             source: "qrc:///image/goBack.png"
             width: parent.width
             height: parent.height
             fillMode: Image.PreserveAspectFit
         }
     }

     // 播放按钮
     Button {
         id: isPlayBtn
         width: root.controlAreaButtonWidth
         height: root.controlAreaButtonHeight
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.top: openFileBtn.top
         contentItem: Image {
             id: isPlayBtnImg
             source: "qrc:///image/isPlay.png"
             width: parent.width
             height: parent.height
             fillMode: Image.PreserveAspectFit
         }
     }
}
