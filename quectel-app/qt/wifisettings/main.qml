import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.VirtualKeyboard 2.2

import ModelManager 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 400
    height: 800
    title: qsTr("WLAN")

    property string hotspot_ssid : 'none';
    property string networkEvent : 'disconnect';

    ModelManager {
        id: modelMgr

        onSignalNetworkEventChanged: {
            myPopup.close()
            if (event == "Connected") {
                listview_id.currentIndex = 0
            }
        }
    }

    Rectangle{
        id:titlebar
        visible: true
        width: parent.width
        height: 150
        z:3
//        opacity: 1
//        color:"red"
//        Item{
//            id:item
//            anchors.fill: parent
//            opacity:1

//            Label{
//                anchors.centerIn: parent
//                text:"hello world!!"
//                font.pixelSize: 27
//            }
//        }
        ColumnLayout {
            anchors.fill: parent
            spacing: 8

            Rectangle{
                height:30  //高
            }

            RowLayout {
//                spacing: parent.width/2 + 50

                Text {
                    Layout.leftMargin: 40
                    font.pixelSize: 30
                    Layout.preferredWidth: titlebar.width * 2/3
                    text: "WLAN"
                }

                Switch {
                    id:switch_btn
                    Layout.fillWidth: true
                    anchors.right: parent.right
                    checked : true
                    onClicked : {
                        if(switch_btn.checked) {
                            modelMgr.enableWlanNetwork()
                        } else {
                            myPopup.close();
                            prompt_id.close()
                            modelMgr.disableWlanNetwork()
                        }
                    }
                }
            }

            Rectangle{
                height:20
//                width: titlebar.width
                Layout.fillWidth: true
                color:"gainsboro"
            }
        }
    }


    NumberAnimation {
        id:ani1
        target: listview_id
        property: "contentY"
        duration: 200
        to:-listview_id.headerItem.height
        running: false
        easing.type: Easing.InOutQuad
    }

    NumberAnimation {
        id:ani2
        target: listview_id
        property: "contentY"
        duration: 200
        to:-titlebar.height
        running: false
        easing.type: Easing.InOutQuad
    }

//    Component {
//        id: wlanModel
//        ListModel {
//            ListElement {
//                hotspotSSID: "quectel1";
//                hotspotRSSI: "3"
//            }
//            ListElement {
//                hotspotSSID: "quectel2";
//                hotspotRSSI: "2"
//            }
//            ListElement {
//                hotspotSSID: "quectel3";
//                hotspotRSSI: "1"
//            }
//        }
//    }

    Component {
        id: headerView
        Item {
            width: listview_id.width
            height: 200
            Label{
                id:label
                text:"Wlan available list"
                font.pixelSize: 20
//                color:"white"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
            }
//            RowLayout {
//                anchors.bottom: parent.bottom
//                anchors.bottomMargin: 50
//                spacing: 8
//                Text {
//                    text: "Name"
//                    font.bold: true
//                    font.pixelSize: 20
//                    Layout.preferredWidth: 120
//                }
//                Text {
//                    text: "Cost"
//                    font.bold: true
//                    font.pixelSize: 20
//                    Layout.preferredWidth: 80
//                }
//                Text {
//                    text: "Manufacturer"
//                    font.bold: true
//                    font.pixelSize: 20
//                    Layout.fillWidth: true
//                }
//            }
        }
    }

//    Component {
//        id: wlanDelegate
//        Item {
//            id: wrapper
//            width: listview_id.width
//            height:50

//            MouseArea {
//                anchors.fill: parent
//                onClicked: {
//                    wrapper.ListView.view.currentIndex = index
//                }

//                onDoubleClicked: {
////                    wrapper.ListView.view.model.remove(index)
//                }
//            }

//            ColumnLayout {
//                spacing: 8
//                RowLayout {
////                    anchors.left: parent.left
////                    anchors.verticalCenter: parent.verticalCenter
//                    spacing: 100

//                    Text {
//                        text: hotspotSSID
//                        color: wrapper.ListView.isCurrentItem ? "red":"black"
//                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22:18
//                        Layout.preferredWidth: 300
//                    }

//                    Text {
//                        text: hotspotRSSI
//                        color: wrapper.ListView.isCurrentItem ? "red":"black"
//                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22:18
//                        Layout.fillWidth: true
//                    }
//                }

//                Label{
//                    text:"Encryption"
//                    font.pixelSize: 10
//                }

//                Rectangle{
//                    id:line_id
//                    height:1
////                    width: wrapper.width
//                    width: 800
//                    color:"gainsboro"
//                }
//            }
//        }
//    }

    //footer view
    Component {
        id: footerView
        Item {
            id: footerRootItem
            width: listview_id.width
            height: 30
            anchors.top: listview_id.bottom
            anchors.topMargin: 30
//            property alias text: footer_txt.text
            signal add()

            RowLayout {
                spacing: 40
//                anchors.right: parent.right
//                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id:footer_txt
                    font.italic: true
                    text:"Add new network"
                    color: "blue"
                    verticalAlignment: Text.AlignVCenter
                }

                Button {
                    text: qsTr("Exit")
//                    background:Rectangle{
//                        anchors.fill: parent
//                        color: "red"
//                    }
                    onClicked: {
                        Qt.quit()
                    }
                }
            }
        }
    }

    ListView{
        id:listview_id
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: 40
        focus: true

//        model: wlanModel.createObject(listview_id)
        model: modelMgr.objectModel()

        onContentYChanged:{
//            if(listview_id.contentY < -titlebar.height){
//                titlebar.opacity = 1-(-listview_id.contentY - titlebar.height)/100.
//                titlebar.y = -listview_id.contentY - titlebar.height
//            }
//            else{
//                item.opacity = 1
//                titlebar.y = 0
//            }
        }

        onMovementEnded: {
//            if(listview_id.contentY < -listview_id.headerItem.height/2.){
//                ani1.running = true
//            } else if(listview_id.contentY < -titlebar.height){
//                ani2.running = true
//            }
        }

//        header:Rectangle{
//            width: listview_id.width
//            height: 200
////            color:"red"
//            Label{
//                id:label
//                text:"this is header"
//                font.pixelSize: 27
////                color:"white"
//                anchors.horizontalCenter: parent.horizontalCenter
//                anchors.bottom: parent.bottom
//                anchors.bottomMargin: 50
//            }

//        }

        header: headerView


//        delegate:Rectangle{
//            id:delegate
//            width: listview_id.width
//            height: 50
//            border.width: 1
//            Label{
//                anchors.fill: parent
//                text: model.modelData.hotspotSSID + model.modelData.hotspotRSSI
//            }
//        }
//        delegate: wlanDelegate
        delegate: Item {
            id: wrapper
            width: listview_id.width
            height:50

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    wrapper.ListView.view.currentIndex = index
                }

                onDoubleClicked: {
                    hotspot_ssid = modelMgr.getHotspotSSID(index)
                    if (modelMgr.checkHotspotConnect(hotspot_ssid) !== 0) {
                        if (modelMgr.selectEnableNetwork(hotspot_ssid) === 0)
                            return;
                        myPopup.open()
                    } else {
                        prompt_id.open()
//                    modelMgr.getHotspotElement(index)
//                    console.log("fulinux:" + modelMgr.getHotspotSSID(index))
                    }
                }
            }

            ColumnLayout {
                spacing: 8
                RowLayout {
//                    anchors.left: parent.left
//                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 100
                    anchors.topMargin: 3

                    ColumnLayout {
                        spacing: 2

                        Layout.preferredWidth: 300
                        Text {
    //                        text: hotspotSSID
                            text: model.modelData.hotspotSSID
                            color: wrapper.ListView.isCurrentItem ? "red":"black"
                            font.pixelSize: wrapper.ListView.isCurrentItem ? 22:18
                        }

                        Label{
                            text:model.modelData.hotspotState
                            color: wrapper.ListView.isCurrentItem ? "red":"black"
                            font.pixelSize: 12
                        }
                    }

                    Text {
//                        text: hotspotRSSI
                        text: model.modelData.hotspotRSSI
                        color: wrapper.ListView.isCurrentItem ? "red":"black"
                        font.pixelSize: wrapper.ListView.isCurrentItem ? 22:18
                        Layout.fillWidth: true
                        Layout.topMargin: 3
                    }
                }

                Rectangle{
                    id:line_id
                    height:1
//                    width: wrapper.width
                    width: 800
                    color:"gainsboro"
                }
            }
        }

//        footer: Rectangle{
//            id:foot
//            color: "green"
//            width: parent.width
//            height: 50
//        }
        footer: footerView

        function addOne() {
//            model.modelData.append({
//                             "hotspotSSID": "quectel4",
//                             "hotspotRSSI": "3"
//                        })
            modelMgr.insert();
        }

        Component.onCompleted: {
            var t_h = listview_id.model.count * 50 + titlebar.height
            if(t_h < listview_id.height){
                listview_id.footerItem.height = listview_id.height - t_h
            }

            listview_id.footerItem.add.connect(listview_id.addOne)
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }

    Popup
    {
        id:myPopup
        width: window.width
        height: window.height/3
        y:(window.height - height) /2
//        z:4
//        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        Item {
            anchors.fill: parent
            ColumnLayout {
                spacing: 10

                Rectangle{
                    height:30  //高
                }

                Text {
                    Layout.leftMargin: 40
                    font.pixelSize: 30
                    text: qsTr(hotspot_ssid)
                }

                Rectangle{
                    height:10
                    width: parent.width
//                    Layout.fillWidth: true
                    color:"gainsboro"
                }

                RowLayout {
                    spacing: 40

                    Layout.leftMargin: 40
                    TextField {
                        id:password_id
                        placeholderText: qsTr("Enter password")
                    }

                    Button{
                        text: qsTr("Confirme")
                        onClicked: {
                            if (password_id.text.length === 0)
                                return
                            modelMgr.connectHotspot(hotspot_ssid, password_id.text)
                            myPopup.close()
                        }
                    }
                }
            }

            Button {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5
                text: "Cancel"
                onClicked: {
                    myPopup.close()
                }
            }
        }
    }

    Popup {
        id:prompt_id
        width: window.width
        height: window.height/4
//        x: (window.width - width)/2
        y: (window.height - height)/2
//        z:4
//        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        Item {
            anchors.fill: parent
            ColumnLayout {
                spacing: 10

                Rectangle{
                    height:50  //高
                }

                Text {
                    Layout.leftMargin: 40
                    font.pixelSize: 15
                    text: hotspot_ssid + qsTr(" has been connected.")
                }

                Rectangle{
                    height:10
                    width: window.width
                    color:"gainsboro"
                }
            }

            RowLayout {
                spacing: 50

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 15

                Button {
                    text: "Connect"
                    onClicked: {
                        modelMgr.selectEnableNetwork(hotspot_ssid)
                        prompt_id.close()
                    }
                }

                Button {
                    text: "Disconnect"
                    onClicked: {
                        modelMgr.selectDisableNetwork(hotspot_ssid)
                        prompt_id.close()
                    }
                }

                Button {
                    text: "Cancel"
                    onClicked: {
                        prompt_id.close()
                    }
                }
            }
        }
    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
