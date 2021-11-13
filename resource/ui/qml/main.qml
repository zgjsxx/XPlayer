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
    title: qsTr("MuYing Player")
    property bool fullscreen: false


    //背景图片
    Image {
        id: backGround
        width: parent.width
        height: parent.height
        source: myplay.backGround ? "file:///" + myplay.backGround : ""
        //fillMode: Image.TileHorizontally
        smooth: true
    }
}
