<%--
EE 461L HW 1 - Blog
Author: Ben Fu (byf69)
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="com.google.appengine.api.users.UserService" %>
<%@ page import="com.google.appengine.api.users.UserServiceFactory" %>
<%@ page import="com.google.appengine.api.users.User" %>
<%@ page import="java.util.List" %>
<%@ page import="com.googlecode.objectify.ObjectifyService" %>
<%@ page import="com.blog.Post" %>
<%@ page import="java.text.DateFormat" %>
<%@ page import="java.text.SimpleDateFormat" %>
<%@ page import="java.util.TimeZone" %>

<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<html>
<head>
    <link href="bootstrap-3.3.6-dist/css/bootstrap.min.css" rel="stylesheet" type="text/css" />
    <script src="jquery-2.2.0.js" type="text/javascript"></script>
    <script src="bootstrap-3.3.6-dist/js/bootstrap.min.js" type="text/javascript"></script>
    <title>Your Posts</title>
</head>
<body>
<nav class="navbar navbar-inverse" style="color: darkslateblue;">
    <div class="container-fluid">
        <div class="navbar-header">
            <button class="navbar-toggle" type="button" data-toggle="collapse" data-target=".bs-navbar-collapse">
                <span class="sr-only">Toggle navigation</span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </button>
            <a href="index.jsp" class="navbar-brand">Ben Fu | EE 461L</a>
        </div>
        <div class="collapse navbar-collapse">
            <form class="navbar-form navbar-right" role="search">
                <div class="form-group">
                    <input type="text" class="form-control" placeholder="Search">
                </div>
                <button type="submit" class="btn btn-default">Submit</button>
            </form>
            <ul class="nav navbar-nav">
                <li class="active"><a href="index.jsp">Home</a></li>
                <li><a href="contact.html">Contact</a></li>
                <li class="dropdown">
                    <%
                        UserService userService = UserServiceFactory.getUserService();
                        User user = userService.getCurrentUser();
                        if (request.getUserPrincipal() != null) {
                    %>
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">
                        <%= user.getNickname() %>
                        <span class="caret"></span></a>
                    <ul class="dropdown-menu">
                        <li><a href="post.html">Write a New Post</a></li>
                        <li><a href="#">Your Posts</a></li>
                        <li role="separator" class="divider"></li>
                        <li><a href="<%= userService.createLogoutURL("/index.jsp") %>">Sign Out</a></li>
                    </ul>
                    <%
                    }
                    else {
                    %>
                    <a href="<%= userService.createLoginURL(request.getRequestURI()) %>">Sign in</a>
                    <%
                        }
                    %>
                </li>
            </ul>
        </div>
    </div>
</nav>
<div class="row" style="width: 100%;">
    <div class="col-lg-2">
        <%
            String sub = (String) request.getAttribute("subscribed");
            if (sub == null) {
        %>
        <form class="form-inline" action="subscribe" method="post" style="padding-left: 1em; padding-top: 2em;">
            <div class="form-group">
                <input type="email" class="form-control input-sm" name="email" value="" placeholder="Email" required>
            </div>
            <div class="form-group">
                <input type="submit" class="btn btn-default btn-sm" value="Subscribe" />
            </div>
        </form>
        <%
        }
        else {
        %>
        <div style="padding-left: 1em; padding-top: 2em;">
            <strong class="text">Thank you for subscribing!</strong>
        </div>
        <%
            }
        %>
        <small style="padding-left: 1em; padding-top: 2em;"
               data-toggle="popover"
               title="Success!"
               data-content="You've successfully been subscribed.">
            Already subscribed? You can unsubscribe <a href="unsubscribe.html">here</a>
        </small>
    </div>
    <div class="col-lg-8">
        <%
            List<Post> posts = ObjectifyService.ofy()
                    .load()
                    .type(Post.class)
                    .filter("author =", user.getNickname())
                    .order("-date")
                    .list();
            DateFormat df = new SimpleDateFormat("MMMM dd, yyyy 'at' h:mm a");
            df.setTimeZone(TimeZone.getTimeZone("America/Chicago"));
            for (Post post : posts) {
                String dateFmtd = df.format(post.date);
        %>
                <div class="card">
                    <div class="card-block">
                        <h1 class="card-title"><%= post.title %></h1>
                        <small class="card-text" style="color: gray; padding-bottom: 5px;">Written by <%= post.author %> on <%= dateFmtd %></small>
                        <p class="card-text" style="padding-top: 5px;"><%= post.content.getValue() %></p>
                    </div>
                </div>
                <br />
        <%
            }
        %>

    </div>

</div>

</body>
</html>
