import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import zydd 1.0

Window {
    id: root
    visible: true
    width: 800
    height: 512
    title: qsTr("Fluid")

    Row {
        FluidSim {
            id: sim
            width: Math.min(root.width,root.height)
            height: Math.min(root.width,root.height)
            g: Math.pow(g.value,3)
            k: Math.pow(k.value,2)
            v: v.value
            dt: 0.001+0.999*Math.pow(dt.value,2)
            factor: Math.round(1+99*Math.pow(factor.value,2))
        }

        Column {
            RowLayout {
                Slider { id: g; from: -1; to: 1; stepSize: 0.01; value: Math.pow(0.07,1/3) }
                Label { text: 'g: ' + sim.g }
            }

            RowLayout {
                Slider { id: k; from: 0; to: 10; stepSize: 0.01; value: 2 }
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
            }

            Button {
                text: 'Restart'
                onPressed: sim.reset()
            }
        }
    }
}
