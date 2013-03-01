<TeXmacs|1.0.7.18>

<style|generic>

<\body>
  <doc-data|<doc-title|Plans and ideas for remote computations>>

  <section|Remote file system>

  <subsection|Main design>

  The remote file system is a so called ``database file system''. The main
  server side object is called a<nbsp>``resource''. We maintain a database
  with properties of resources. Each property is currently a<nbsp>triple
  <scm|(rid attr val)>, where <verbatim|rid> is the unique identifier of the
  resource, <verbatim|attr> an attribute and<nbsp><verbatim|val> the
  corresponding value. An attribute can have several values.

  One special attribute is <verbatim|type>. There are currently three types
  of resources: <verbatim|user>, <verbatim|file> and <verbatim|dir>. Each
  resource should also admit the attribute <verbatim|name> and
  <verbatim|owner>. Special types of resources may admit additional specific
  attributes.

  <subsection|File management>

  The files and directories are organized in a usual directory structure, but
  they may admit addional attributes in order to facilitate searching (not
  yet implemented). The <verbatim|name> attribute of files and directories is
  the traditional file/directory name, whereas the <verbatim|dir> attribute
  specifies the directory (except for roots). Any user with a given
  <verbatim|pseudo> automatically acquire ownership over a few special
  directories, such as the home directory <verbatim|~pseudo>. The special
  attribute <verbatim|location> also specifies the physical location on the
  server side where the file is saved. An attributed <verbatim|mime> for the
  mime type of the file is planned for later.

  <subsection|Access rights>

  Currently, there are three types of access rights: <verbatim|owner>
  (ownership with read/write permission and the right to change the
  properties), <verbatim|readable> (read permission) and <verbatim|writable>
  (write permission).

  In addition, there are special virtual users which are called groups, which
  are really collections of other users (or groups); a member of the group is
  specified with the <verbatim|member> attribute. If a group is specified for
  a particular permission, then all its members have that permission. There
  is also one special group <verbatim|all>, which contains all users of the
  server.

  <subsection|Plans for versioning>

  The abstract way that we would wish to regard versioning is that a
  versioned file system (not yet implemented) may be contemplated using
  different views. Any view on the versioned file system results in an
  ordinary file system (implemented as described above). The file system
  consists of two parts: a database with <verbatim|(rid attr val)> entries,
  and attached files with bulk data. The view should therefore specify how to
  generate these two parts, at least virtually.

  For history management, the idea would be to use a database whose entries
  contain two additional fields <verbatim|start> and <verbatim|end>
  corresponding to the start and end dates of a property in the database. For
  instance <verbatim|(xy14z name test.tm 2008-08-02 2009-12-11)> might
  specify that the entry <verbatim|(xy14z name test.tm)> was valid between
  <verbatim|2008-08-02> and <verbatim|2009-12-11>.

  For branches (typically of projects), in which users may have their own
  modified versions of resources, the idea would be to regard the branch as a
  patch applied to the entire file system. On the one hand, this means that
  we have to patch the database. This is done through the introduction of
  additional fields <verbatim|branch> and <verbatim|direction> for database
  entries. The <verbatim|direction> (<verbatim|insert> or <verbatim|remove>)
  specifies whether an entry should be added to or removed from the unpatched
  database. On the other hand, the attached files to the database may have to
  be patched as well. This is done through two special attributes
  <verbatim|patcher> and <verbatim|patch>. The <verbatim|patcher> specifies a
  program or way which will be used for doing the patch (to the unpatched
  file at <verbatim|location>), and <verbatim|patch> contains the patch
  itself.

  Of course, views can be composed: we might apply the combined patches of
  several branches, or the patch corresponding to a past version of a branch
  to a past version of the file system.

  <section|User interface>

  <section|Minor points>
</body>

<initial|<\collection>
</collection>>

<\references>
  <\collection>
    <associate|auto-1|<tuple|1|?>>
    <associate|auto-2|<tuple|1.1|?>>
    <associate|auto-3|<tuple|1.2|?>>
    <associate|auto-4|<tuple|1.3|?>>
    <associate|auto-5|<tuple|1.4|?>>
    <associate|auto-6|<tuple|2|?>>
    <associate|auto-7|<tuple|3|?>>
  </collection>
</references>