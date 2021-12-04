const fs = require('fs');

const app = require('express')();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const port = process.env.PORT || 3030;

var mqtt = require('mqtt')
var client = mqtt.connect('mqtt:192.168.1.6:1883')
var topic = 'infoTopic'

//DB
const { Client } = require('pg')
const dbclient = new Client({
  user: 'pi',
  host: '192.168.1.6',
  database: 'test',
  password: 'dayiotdb',
  port: 5432,
})
process.env.TZ = 'Europe/Rome'

dbclient.connect()
const qtext0 = 'insert into dayinfo(time, mode, water, umidity, temperature, light, ttime) select T.current_timestamp, J.mode, J.water, J.umidity, J.temperature, J.light, T.current_timestamp-T.startTime from json_populate_record(NULL::dayinfo, \''
const qtext1 = 'insert into dayinfo(time, mode, water, umidity, temperature, light, ttime) select current_timestamp, J.mode, J.water, J.umidity, J.temperature, J.light, J.ttime from json_populate_record(NULL::dayinfo, \''
//const qinfo = 'select S.count, S.sum + current_timestamp - dayinfo.time as time from ( select time from dayinfo where water = 0 order by time desc limit 1 ) as t1, dayinfo, (select COUNT(ttime), SUM(ttime) from dayinfo where water = 0 group by water) as S where dayinfo.time > t1.time order by dayinfo.time asc limit 1;'
//const qinfo = 'select P.count, (CASE    WHEN S.interv is NULL THEN P.sum    ELSE P.sum + S.interv END) as time from (select COUNT(D.ttime), SUM(D.ttime) from dayinfo D where water = 0 group by water) as P, (select coalesce ((select current_timestamp - dayinfo.time as interv  from ( select time from dayinfo where water = 0 order by time desc limit 1 ) as t1,  dayinfo where dayinfo.time > t1.time order by dayinfo.time asc limit 1), NULL) as interv) as S;'
//const qinfo = 'select F.count, F.time, EXTRACT(EPOCH FROM F.time) as seconds from (select P.count, (CASE WHEN S.interv is NULL THEN P.sum ELSE P.sum + S.interv END) as time from (select COUNT(D.ttime), SUM(D.ttime) from dayinfo D where water = 0 group by water) as P, (select coalesce ((select current_timestamp - dayinfo.time as interv  from ( select time from dayinfo where water = 0 order by time desc limit 1 ) as t1,  dayinfo where dayinfo.time > t1.time order by dayinfo.time asc limit 1), NULL) as interv) as S) as F;'
const qinfo = 'select F.count, F.time, EXTRACT(EPOCH FROM F.time) as seconds from (select P.count, (CASE WHEN S.interv is NULL THEN P.sum WHEN P.sum is NULL THEN S.interv ELSE P.sum + S.interv END) as time from (select COUNT(D.ttime), SUM(D.ttime) from dayinfo D where water = 0 group by water) as P, (select coalesce ((select current_timestamp - dayinfo.time as interv  from ( select time from dayinfo where water = 0 order by time desc limit 1 ) as t1,  dayinfo where dayinfo.time > t1.time order by dayinfo.time asc limit 1), NULL) as interv) as S) as F;'

client.on('message', (topic, message) => {
    if(message) {
        try {
            var mess = JSON.parse(message)
//            console.log(mess)
            messData = JSON.stringify(mess)
            var querytext
            if (mess.water == 0) {
              querytext  = qtext0+messData+'\') as J, (select current_timestamp, coalesce ((select dayinfo.time as startTime from ( select time from dayinfo where water = 0 order by time desc limit 1 ) as t1, dayinfo where dayinfo.time > t1.time order by dayinfo.time asc limit 1), NULL) as startTime) as T;' 
            } else if (mess.water = 1) {
              querytext = qtext1+messData+'\') as J;'
            } else {
              alert("error")
            }
            dbclient.query(querytext, (err, res) => {
              if(err!=null) {
                console.log(err)
              }
            })
            dbclient.query(qinfo, (err, res) => {
              mess['time'] = res.rows[0].time
              mess['count'] = res.rows[0].count
              mess['seconds'] = res.rows[0].seconds
            //      console.log(mess)
              io.emit('chat message', mess)
            })
        } catch(e) {
            console.log("error parsing json")
            //alert(e); // error in the above string (in this case, yes)!
        }
    }

    
    //io.emit('chat message', mess)
})

client.on('connect', () => {
    client.subscribe(topic)
})



app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', (socket) => {
  socket.on('chat message', msg => {
//    console.log(msg)
    client.publish('controlTopic', msg)
  });
});

http.listen(port, () => {
  console.log(`Socket.IO server running at http://localhost:${port}/`);
});

