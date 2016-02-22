package com.blog;

import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;
import com.googlecode.objectify.annotation.Index;

/**
 * Created by Ben on 2/18/2016.
 */
@Entity

public class Subscriber {
    @Id public Long id;
    @Index public String email;

    public Subscriber() {}

    public Subscriber(String email) {
        this.email = email;
    }
}
