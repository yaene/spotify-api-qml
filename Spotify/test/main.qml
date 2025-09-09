import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Spotify

Window {
    id: root
    visible: true

    property Playlist selectedPlaylist: null

    SpotifyApi {
        id: api
        clientId: "d66dea877b004271813e5579438321df"
        scope: ["user-read-playback-state", "user-modify-playback-state", "playlist-read-private", "user-library-read"]
        Component.onCompleted: api.init()
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Text {
            text: api.authorized ? "Authorized" : "Not authorized"
        }
        Text {
            text: `User: ${JSON.stringify(api.currentUser, null, 2)}`
            wrapMode: Text.Wrap
            Layout.preferredWidth: 300
            font.pixelSize: 12
        }
        RowLayout {
            spacing: 8
            Button {
                text: "Update Current User"
                onClicked: api.updateCurrentUser()
            }
            Button {
                text: "Authorize"
                onClicked: api.authorize()
            }
            Button {
                text: "Show Playlists"
                onClicked: api.updatePlaylists()
            }
            Text {
                text: api.playlists.length
            }
        }

        Rectangle {
            id: playlistRect
            width: 380
            height: 200
            color: "#181933"
            radius: 8
            border.color: "#242443"
            border.width: 1
            visible: api.playlists.length > 0
            ListView {
                id: playlistList
                anchors.fill: parent
                model: api.playlists
                clip: true
                delegate: Rectangle {
                    width: playlistList.width
                    height: 40
                    color: ListView.isCurrentItem ? "#3638a8" : (index % 2 === 0 ? "#2a2b44" : "#232345")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.selectedPlaylist = modelData;
                            if (root.selectedPlaylist) {
                                root.selectedPlaylist.fetchTracks();
                            }
                        }
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 12
                        text: modelData.name
                        color: "white"
                        font.pixelSize: 16
                    }
                }
            }
        }

        Rectangle {
            width: 380
            height: 200
            color: "#181933"
            radius: 8
            border.color: "#442442"
            border.width: 1
            visible: root.selectedPlaylist !== null && root.selectedPlaylist.tracks.length > 0
            ListView {
                id: trackList
                anchors.fill: parent
                clip: true
                model: root.selectedPlaylist ? root.selectedPlaylist.tracks : []
                delegate: Rectangle {
                    width: trackList.width
                    height: 32
                    color: index % 2 == 0 ? "#214" : "#123"
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 12
                        text: modelData
                        color: "white"
                        font.pixelSize: 14
                    }
                }
            }
        }
    }
}
