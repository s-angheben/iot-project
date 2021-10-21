drop table if exists dayinfo;

set timezone='Europe/Rome';

create table dayinfo (
    time TIMESTAMP PRIMARY KEY,
    mode smallint,
    water smallint NOT NULL,
    umidity real,
    temperature real,
    light real,
    ttime interval
);

insert into dayinfo(time, mode, water, umidity, temperature, light, ttime) 
    values (current_timestamp, NULL, 0, NULL, NULL, NULL, NULL);
