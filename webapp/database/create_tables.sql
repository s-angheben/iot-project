drop table if exists dayinfo;

set timezone='Europe/Rome';

create table dayinfo (
    time TIMESTAMP PRIMARY KEY,
    mode smallint,
    water smallint NOT NULL,
    humidity real,
    temperature real,
    light real,
    ttime interval
);

insert into dayinfo(time, mode, water, humidity, temperature, light, ttime) 
    values (current_timestamp, NULL, 0, NULL, NULL, NULL, NULL);
