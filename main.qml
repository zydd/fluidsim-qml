import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import zydd.fluid 1.0

Item {
    id: root
    visible: true
    width: 640*1366/768
    height: 640

    FluidSim {
        id: sim
//            width: height; height: Math.min(root.width,root.height)
        anchors.fill: parent
        g: g.value
        k: k.value
        v: v.value
        dt: 0.001+0.999*Math.pow(dt.value,2)
        factor: Math.round(1+99*Math.pow(factor.value,2))
        simw: 256*1366/768
        simh: 256
        running: true
        focus: true
        display: 0

        property int button: 0

        MouseArea {
            id: ma
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onDoubleClicked: sim.reset()
            onPressed: {
                sim.button = mouse.button
                sim.running = true
            }

            onReleased: {
                if (sim.button === mouse.button)
                    sim.button = 0;
            }

            onPressAndHold: {
                if (mouseX < 10 && mouseY < 10)
                    config.visible = true
            }
        }

        Timer {
            running: ma.pressed
            repeat: true
            interval: 16
            onTriggered: {
                stopsim.restart()
                sim.den_ellipse(Qt.point(ma.mouseX,ma.mouseY),brush.value,
                                sim.button === Qt.RightButton ? 2 : -2)
            }
        }

        Timer {
            id: stopsim
            running: false
            interval: 20000
            onTriggered: sim.running = false
        }
    }

    Keys.onPressed: {
        if (event.text >= '1' && event.text <= '6')
            sim.display = Number.fromLocaleString(Qt.locale("de_DE"), event.text)-1
        switch(event.text) {
        case '\'': config.visible = !config.visible; break;
        }
    }

    Row {
        id: config
        visible: false
        Column {
            RowLayout {
                Slider { id: g; from: -1; to: 1; stepSize: 0.01; value: 0.07 }
                Label { text: 'g: ' + sim.g }
            }

            RowLayout {
                Slider { id: k; from: 0; to: 15; stepSize: 0.01; value: 2 }
                Label { text: 'k: ' + sim.k }
            }

            RowLayout {
                Slider { id: v; from: 0; to: 2; stepSize: 0.001; value: 0.05 }
                Label { text: 'v: ' + v.value }
            }

            RowLayout {
                Slider { id: dt; from: 0; to: 1; stepSize: 0.001; value: Math.sqrt((0.05-0.001)/0.999) }
                Label { text: 'dt: ' + sim.dt }
            }

            RowLayout {
                Slider { id: factor; from: 0; to: 1; value: Math.sqrt(7/100) }
                Label { text: 'factor: ' + sim.factor }
            }

            RowLayout {
                RadioButton { onPressed: sim.display = 0; checked: true }
                RadioButton { onPressed: sim.display = 1; }
                RadioButton { onPressed: sim.display = 2 }
                RadioButton { onPressed: sim.display = 3 }
                RadioButton { onPressed: sim.display = 4 }
                RadioButton { onPressed: sim.display = 5 }
            }
            RowLayout {
                Slider { id: brush; from: 0; to: 1; stepSize: 0.001; value: 0.05 }
                Label { text: 'brush: ' + brush.value }
            }

            RowLayout {
                Button {
                    text: 'Reset'
                    onPressed: {
                        sim.reset()
                        sim.running = false
                    }
                }

                Button {
                    text: 'Hide'
                    onPressed: config.visible = false
                }
            }
        }
    }
}
