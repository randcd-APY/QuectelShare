import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.VirtualKeyboard 2.2

import NetworkDetector 1.0

ApplicationWindow {
    id: window
    visible: true
    title: qsTr("Network Detector")

    NetworkDetector {
        id: networkDetector_id
        /*
        #define NETWORK_BEGINE_STEP             0
        #define NETWORK_INIT_START              1
        #define NETWORK_INIT_SUCCESSED          2
        #define NETWORK_INIT_FAILED             3
        #define NETWORK_ENABLE_START            4
        #define NETWORK_ENABLE_SUCCESSED        5
        #define NETWORK_ENABLE_FAILED           6
        #define NETWORK_CONNECT_START           7
        #define NETWORK_CONNECT_SUCCESSED       8
        #define NETWORK_CONNECT_FAILED          9
        #define NETWORK_CONNECT_BREAK           10
        #define NETWORK_DISCONNECT_START        11
        #define NETWORK_DISCONNECT_SUCCESSED    12
        #define NETWORK_DISCONNECT_FAILED       13
        #define NETWORK_BLINK_START             14
        #define NETWORK_BLINK_END               15
        */
        onNetworkStateChanged: {
            switch(networkState) {
            case 0:
                switch1.enabled = false
                color2_id.color = "black"
                txt_state_id.text = "Network: Begin"
                break;
            case 1:
                switch1.enabled = false
                color2_id.color = "antiquewhite"
                txt_state_id.text = "Network: Init start..."
                break;
            case 2:
                color2_id.color = "burlywood"
                txt_state_id.text = "Network: Init successed"
                break;
            case 3:
                switch1.enabled = true
                color2_id.color = "coral"
                txt_state_id.text = "Network: Init failed"
                break;
            case 4:
                color2_id.color = "deepskyblue"
                txt_state_id.text = "Network: Enable start..."
                break;
            case 5:
                color2_id.color = "mediumblue"
                txt_state_id.text = "Network: Enable successed"
                break;
            case 6:
                switch1.enabled = true
                color2_id.color = "magenta"
                txt_state_id.text = "Network: Enable failed"
                break;
            case 7:
                switch1.enabled = false
                color2_id.color = "mediumseagreen"
                txt_state_id.text = "Network: Connect start..."
                break;
            case 8:
                switch1.enabled = true
                switch1.checked = true
                color2_id.color = "magenta"
                txt_state_id.text = "Network: Connect successed"
                break;
            case 9:
                switch1.enabled = true
                switch1.checked = true
                color2_id.color = "mediumvioletred"
                txt_state_id.text = "Network: Connect failed"
                break;
            case 10:
                switch1.enabled = true
                color2_id.color = "red"
//                txt_state_id.text = "Network: Fault..."
                break;
            case 11:
                switch1.enabled = false
                color2_id.color = "khaki"
                txt_state_id.text = "Network: Disconnect start..."
                break;
            case 12:
                switch1.enabled = true
                switch1.checked = false
                color2_id.color = "orange"
                txt_state_id.text = "Network: Disconnect successed"
                break;
            case 13:
                switch1.enabled = true
                switch1.checked = false
                color2_id.color = "crimson"
                txt_state_id.text = "Network: Disconnect failed"
                break;
            case 14:
                color2_id.color = "lime"
//                txt_state_id.text = "Network: Access.."
                break;
            case 15:
                color2_id.color = "limegreen"
//                txt_state_id.text = "Network: Access..."
                break;
            default:
                switch1.enabled = true
                color2_id.color = "red"
                txt_state_id.text = "Network: Error"
                break;
            }
        }
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.leftMargin: 50
        spacing: 8

        Rectangle{
            height:50  //高
        }

        Text {
            font.pixelSize: 40
            text: "Network Detector"
        }

        Rectangle{
            height:60  //高
        }

        Switch {
            id:switch1
            text: qsTr("Mobile Network")
            font.pixelSize: 30
            checked : networkDetector_id.bswitch
            onClicked : {
//                switch1.enabled = false
                if(switch1.checked) {
                    networkDetector_id.switchNetworkState(true)
                } else {
                    networkDetector_id.switchNetworkState(false)
                }
//                switch1.enabled = true
            }
        }

        Rectangle{
            height:60  //高
        }

        Rectangle{//模拟线段
            height:1  //高
            width: 800
            color:"red" //颜色
        }

        Text {
            id:txt_state_id
            font.pixelSize: 30
            visible: (text.length == 0) ? false:true
            text: "Network: Disconnect successed"
        }

        RowLayout {
            spacing: 40
            Text {
                font.pixelSize: 30
                text: "Ping " + networkDetector_id.address
            }

            Rectangle{
                y: 100; width: 30; height: 30
                radius: 15
                gradient: Gradient {
                  GradientStop { position: 0.0; color: "lightsteelblue" }
                  GradientStop {
                      id: color2_id
                      position: 1.0;
                      color: "darkgrey"
                  }
                }
            }
        }

        Text {
            font.pixelSize: 30
            visible: (networkDetector_id.pingAvgTime.length == 0) ? false:true
            text: networkDetector_id.pingAvgTime
        }

        Text {
            font.pixelSize: 30
            visible: (text.length == 0) ? false:true
            text: networkDetector_id.primaryDNS
        }

        Text {
            font.pixelSize: 30
            visible: (text.length == 0) ? false:true
            text: networkDetector_id.secondDNS
        }

        Rectangle{//模拟线段
            height:1  //高
            width: 800
            color:"red" //颜色
        }

        Rectangle{
            height:60  //高
        }

        RowLayout {
            spacing: 40
            TextField {
                id:networkAddress_id
                placeholderText: qsTr("Enter network address")
                validator: RegExpValidator{id:regexp ; regExp: /^www.(.*).com$|(([1-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.)(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){2}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/ }
            }

            Button{
                text: qsTr("confirme")
                onClicked: {
//                    console.log(networkAddress_id.text)
                    if (networkAddress_id.text.length === 0)
                        return
                    networkDetector_id.address = networkAddress_id.text
                }
            }
        }
    }

    Button {
        width: 120
        height: 80
        text: qsTr("Exit")
        anchors {bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 20 }
        background:Rectangle{
            anchors.fill: parent
            color: "red"
        }

        onClicked: {
            Qt.quit()
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
