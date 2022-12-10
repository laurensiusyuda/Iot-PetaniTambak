import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_flutter/mqtt_client.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter MQTT Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: const MyHomePage(title: 'Flutter MQTT Demo'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({Key? key, required this.title}) : super(key: key);

  final String title;
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  late MqttClient client;
  final topic = 'topicName/salt';
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            ElevatedButton(
              child: Text('Connect'),
              onPressed: () => {
                connect().then((value) {
                  client = value;
                })
              },
            ),
            ElevatedButton(
                child: const Text('Subscribe'),
                onPressed: () =>
                    {client?.subscribe(topic, MqttQos.atLeastOnce)}),
            ElevatedButton(
                child: const Text('Subscribe'),
                onPressed: () =>
                    {client?.subscribe(topic, MqttQos.atLeastOnce)}),
          ],
        ),
      ),
    );
  }
}
